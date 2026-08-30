package io.github.dk64port;

import java.io.*;
import java.nio.file.*;
import java.util.*;
import java.util.zip.*;

/** Bounded extraction into a newly created private directory; never follows ZIP paths outside it. */
final class DriverArchive {
    static final long MAX_BYTES = 256L * 1024 * 1024;
    static final long MAX_FILE_BYTES = 128L * 1024 * 1024;
    static final int MAX_ENTRIES = 128;

    static List<Path> extract(InputStream source, Path directory) throws IOException {
        Path root = directory.toAbsolutePath().normalize();
        if (!Files.isDirectory(root) || Files.isSymbolicLink(root)) throw new IOException("Invalid extraction directory.");
        List<Path> files = new ArrayList<>();
        Set<Path> seen = new HashSet<>();
        long total = 0;
        int entries = 0;
        byte[] buffer = new byte[32768];
        try (ZipInputStream zip = new ZipInputStream(source)) {
            ZipEntry entry;
            while ((entry = zip.getNextEntry()) != null) {
                String name = entry.getName();
                if (++entries > MAX_ENTRIES) throw new IOException("Driver ZIP has too many entries.");
                if (name.isEmpty() || name.length() > 240 || name.startsWith("/") || name.contains("\\")
                        || name.contains(":") || Arrays.asList(name.split("/")).contains("..")) {
                    throw new IOException("Unsafe path in driver ZIP.");
                }
                Path target = root.resolve(name).normalize();
                if (!target.startsWith(root) || target.equals(root) || !seen.add(target)) {
                    throw new IOException("Unsafe or duplicate driver ZIP entry.");
                }
                if (entry.isDirectory()) { Files.createDirectories(target); continue; }
                if (entry.getSize() > MAX_FILE_BYTES) throw new IOException("Driver ZIP entry is too large.");
                Files.createDirectories(target.getParent());
                long size = 0;
                try (OutputStream out = Files.newOutputStream(target, StandardOpenOption.CREATE_NEW)) {
                    int count;
                    while ((count = zip.read(buffer)) != -1) {
                        size += count;
                        total += count;
                        if (size > MAX_FILE_BYTES || total > MAX_BYTES) throw new IOException("Driver ZIP is too large.");
                        out.write(buffer, 0, count);
                    }
                }
                if (name.endsWith(".so")) validateArm64(target);
                files.add(target);
            }
        }
        if (files.isEmpty()) throw new IOException("Empty or invalid driver ZIP.");
        return files;
    }

    static void validateArm64(Path library) throws IOException {
        byte[] header = new byte[64];
        try (DataInputStream input = new DataInputStream(Files.newInputStream(library))) { input.readFully(header); }
        if (header[0] != 0x7f || header[1] != 'E' || header[2] != 'L' || header[3] != 'F'
                || header[4] != 2 || header[5] != 1 || header[6] != 1
                || header[16] != 3 || header[17] != 0 || (header[18] & 255) != 183 || header[19] != 0) {
            throw new IOException("Driver must contain Android ARM64 shared libraries.");
        }
    }

    static void removePrivateTree(Path directory) throws IOException {
        // Callers only pass their own staging directory, never a path obtained from a ZIP.
        try (java.util.stream.Stream<Path> paths = Files.walk(directory)) {
            for (Path path : (Iterable<Path>) paths.sorted(Comparator.reverseOrder())::iterator) Files.deleteIfExists(path);
        }
    }
}

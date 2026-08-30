package io.github.dk64port;

import java.io.*;
import java.nio.file.*;
import java.util.*;
import java.util.zip.*;

/** Private staging only. No archive path is ever used as a destination path. */
final class ModArchive {
    static final long MAX_INPUT = 512L * 1024 * 1024;
    static final long MAX_EXPANDED = 2L * 1024 * 1024 * 1024;
    static final int MAX_ENTRIES = 32768;
    static final int MAX_MANIFEST = 256 * 1024;
    static final class Candidate {
        final Path file;
        final String filename;
        final byte[] manifest;
        Candidate(Path file, String filename, byte[] manifest) {
            this.file = file; this.filename = filename; this.manifest = manifest;
        }
    }
    private static final class Budget { long bytes; int entries; }
    private static final class Scan {
        byte[] manifest;
        boolean textures;
        final List<String> mods = new ArrayList<>();
    }
    static boolean isMod(String name) {
        String lower = name.toLowerCase(Locale.ROOT);
        return lower.endsWith(".nrm") || lower.endsWith(".rtz");
    }
    private static void checkFilename(String name) throws IOException {
        if (name == null || name.length() > 180 || !name.matches("[A-Za-z0-9_][A-Za-z0-9_. ()-]*")
                || name.toLowerCase(Locale.ROOT).endsWith(".offline.nrm")) {
            throw new IOException("Unsupported filename or offline/native mod. Use a standard .nrm or .rtz package.");
        }
    }
    static void copyBounded(InputStream input, OutputStream output, long limit) throws IOException {
        byte[] buffer = new byte[32768];
        long size = 0;
        int count;
        while ((count = input.read(buffer)) != -1) {
            size += count;
            if (size > limit) throw new IOException("Mod file exceeds the import size limit.");
            output.write(buffer, 0, count);
        }
    }
    static Candidate prepare(InputStream input, String name, Path staging) throws IOException {
        checkFilename(name);
        if (!isMod(name) && !name.toLowerCase(Locale.ROOT).endsWith(".zip")) {
            throw new IOException("Choose a .nrm, .rtz or mod ZIP. ROMs, drivers and .apworld files are not recomp mods.");
        }
        Path source = staging.resolve("source");
        try (OutputStream output = Files.newOutputStream(source, StandardOpenOption.CREATE_NEW)) {
            copyBounded(input, output, MAX_INPUT);
        }
        Budget budget = new Budget();
        Scan scan = scan(source, budget);
        if (isMod(name)) return candidate(source, name, scan);
        if (scan.mods.size() != 1) {
            throw new IOException("The ZIP must contain exactly one .nrm or .rtz. For a multi-mod bundle, import its mods separately.");
        }
        String entryName = scan.mods.get(0);
        String filename = entryName.substring(entryName.lastIndexOf('/') + 1);
        checkFilename(filename);
        Path extracted = staging.resolve("mod");
        try (ZipFile zip = new ZipFile(source.toFile()); InputStream stream = zip.getInputStream(zip.getEntry(entryName));
                OutputStream output = Files.newOutputStream(extracted, StandardOpenOption.CREATE_NEW)) {
            copyBounded(stream, output, MAX_INPUT);
        }
        return candidate(extracted, filename, scan(extracted, budget));
    }
    private static Candidate candidate(Path file, String name, Scan scan) throws IOException {
        if (scan.manifest == null && (!name.toLowerCase(Locale.ROOT).endsWith(".rtz") || !scan.textures)) {
            throw new IOException("Not a recomp mod: missing root mod.json (or rt64.json for textures).");
        }
        String normalizedName = name.substring(0, name.length() - 4) + name.substring(name.length() - 4).toLowerCase(Locale.ROOT);
        return new Candidate(file, normalizedName, scan.manifest);
    }
    static byte[] readManifest(Path file) throws IOException {
        try (ZipFile zip = new ZipFile(file.toFile())) {
            ZipEntry entry = zip.getEntry("mod.json");
            if (entry == null) {
                if (file.getFileName().toString().toLowerCase(Locale.ROOT).endsWith(".rtz") && zip.getEntry("rt64.json") != null) return null;
                throw new IOException("Missing mod manifest.");
            }
            try (InputStream input = zip.getInputStream(entry); ByteArrayOutputStream output = new ByteArrayOutputStream()) {
                copyBounded(input, output, MAX_MANIFEST);
                return output.toByteArray();
            }
        }
    }
    private static Scan scan(Path source, Budget budget) throws IOException {
        Scan result = new Scan();
        Set<String> seen = new HashSet<>();
        try (ZipFile zip = new ZipFile(source.toFile())) {
            Enumeration<? extends ZipEntry> entries = zip.entries();
            while (entries.hasMoreElements()) {
                ZipEntry entry = entries.nextElement();
                String name = entry.getName();
                if (++budget.entries > MAX_ENTRIES) throw new IOException("Too many entries in mod package.");
                if (name.isEmpty() || name.length() > 512 || name.startsWith("/") || name.contains("\\") || name.contains(":")) {
                    throw new IOException("Unsafe path in mod package.");
                }
                for (int i = 0; i < name.length(); i++) if (Character.isISOControl(name.charAt(i))) throw new IOException("Unsafe path in mod package.");
                for (String part : name.split("/")) if (part.isEmpty() || part.equals(".") || part.equals("..")) throw new IOException("Unsafe path in mod package.");
                if (!seen.add(name.toLowerCase(Locale.ROOT))) throw new IOException("Duplicate path in mod package.");
                String lower = name.toLowerCase(Locale.ROOT);
                if (lower.endsWith(".dll") || lower.endsWith(".dylib") || lower.endsWith(".so") || lower.contains(".so.")) {
                    throw new IOException("Native-library mod packages are not supported by this importer yet. Desktop libraries cannot run on Android.");
                }
                if (entry.isDirectory()) continue;
                if (entry.getSize() < 0 || entry.getSize() > MAX_EXPANDED - budget.bytes) throw new IOException("Expanded mod package is too large.");
                boolean manifest = name.equals("mod.json");
                if (manifest && entry.getSize() > MAX_MANIFEST) throw new IOException("Mod manifest is too large.");
                ByteArrayOutputStream data = manifest ? new ByteArrayOutputStream() : null;
                CRC32 crc = new CRC32();
                long actual = 0;
                try (InputStream stream = zip.getInputStream(entry)) {
                    byte[] buffer = new byte[32768];
                    int count;
                    while ((count = stream.read(buffer)) != -1) {
                        budget.bytes += count;
                        actual += count;
                        if (budget.bytes > MAX_EXPANDED || (manifest && actual > MAX_MANIFEST)) throw new IOException("Expanded mod package is too large.");
                        crc.update(buffer, 0, count);
                        if (data != null) data.write(buffer, 0, count);
                    }
                }
                if (actual != entry.getSize() || crc.getValue() != entry.getCrc()) throw new IOException("Corrupt mod package entry.");
                if (manifest) result.manifest = data.toByteArray();
                if (name.equals("rt64.json")) result.textures = true;
                if (isMod(name)) result.mods.add(name);
            }
        }
        return result;
    }
}

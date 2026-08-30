package io.github.dk64port;

import java.io.*;
import java.nio.file.*;
import java.security.MessageDigest;

/** ROM validation uses content, not filename extensions; nothing is downloaded. */
final class RomImporter {
    static final int ROM_SIZE = 32 * 1024 * 1024;
    static final String SHA1 = "cf806ff2603640a748fca5026ded28802f1f4a50";

    static byte[] normalize(InputStream input) throws Exception {
        byte[] rom = new byte[ROM_SIZE];
        int offset = 0;
        while (offset < rom.length) {
            int count = input.read(rom, offset, rom.length - offset);
            if (count < 0) throw new IOException("The ROM is shorter than 32 MiB.");
            if (count == 0) continue;
            offset += count;
        }
        if (input.read() != -1) throw new IOException("Expected a 32 MiB ROM, not an archive or decompressed file.");
        int magic = ((rom[0] & 255) << 24) | ((rom[1] & 255) << 16) | ((rom[2] & 255) << 8) | (rom[3] & 255);
        if (magic == 0x37804012) {
            for (int i = 0; i < rom.length; i += 2) swap(rom, i, i + 1);
        } else if (magic == 0x40123780) {
            for (int i = 0; i < rom.length; i += 4) {
                swap(rom, i, i + 3);
                swap(rom, i + 1, i + 2);
            }
        } else if (magic != 0x80371240) {
            throw new IOException("This is not a recognized N64 ROM format.");
        }
        byte[] digest = MessageDigest.getInstance("SHA-1").digest(rom);
        StringBuilder hex = new StringBuilder();
        for (byte value : digest) hex.append(String.format(java.util.Locale.ROOT, "%02x", value & 255));
        if (!SHA1.equals(hex.toString())) throw new IOException("Unsupported ROM. Please use the original US version of DK64.");
        return rom;
    }

    static void importRom(InputStream input, File directory) throws Exception {
        byte[] rom = normalize(input);
        if (!directory.isDirectory() && !directory.mkdirs()) throw new IOException("Cannot create private storage.");
        File temporary = File.createTempFile("rom-", ".tmp", directory);
        try {
            try (FileOutputStream output = new FileOutputStream(temporary)) {
                output.write(rom);
                output.getFD().sync();
            }
            Files.move(temporary.toPath(), new File(directory, "imported.z64").toPath(),
                StandardCopyOption.ATOMIC_MOVE, StandardCopyOption.REPLACE_EXISTING);
        } finally {
            Files.deleteIfExists(temporary.toPath());
        }
    }

    private static void swap(byte[] data, int first, int second) {
        byte saved = data[first]; data[first] = data[second]; data[second] = saved;
    }
}

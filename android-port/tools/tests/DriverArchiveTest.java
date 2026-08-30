package io.github.dk64port;

import java.io.*;
import java.nio.file.*;
import java.util.zip.*;

public final class DriverArchiveTest {
    private static byte[] arm64() {
        byte[] elf = new byte[64];
        elf[0] = 0x7f; elf[1] = 'E'; elf[2] = 'L'; elf[3] = 'F';
        elf[4] = 2; elf[5] = 1; elf[6] = 1; elf[16] = 3; elf[18] = (byte)183;
        return elf;
    }
    private static byte[] archive(String[] names, byte[][] contents) throws IOException {
        ByteArrayOutputStream bytes = new ByteArrayOutputStream();
        try (ZipOutputStream zip = new ZipOutputStream(bytes)) {
            for (int i = 0; i < names.length; i++) {
                zip.putNextEntry(new ZipEntry(names[i])); zip.write(contents[i]); zip.closeEntry();
            }
        }
        return bytes.toByteArray();
    }
    private static void check(Path base, byte[] zip, boolean accept) throws IOException {
        Path staging = Files.createTempDirectory(base, "driver-test-");
        try {
            boolean accepted = false;
            try { DriverArchive.extract(new ByteArrayInputStream(zip), staging); accepted = true; }
            catch (IOException expected) { if (accept) throw expected; }
            if (accepted != accept) throw new AssertionError("Unexpected archive acceptance");
        } finally { DriverArchive.removePrivateTree(staging); }
    }
    public static void main(String[] args) throws IOException {
        Path base = Paths.get(args[0]); Files.createDirectories(base);
        byte[] elf = arm64();
        check(base, archive(new String[]{"driver/meta.json", "driver/libvulkan_freedreno.so"}, new byte[][]{"{}".getBytes(), elf}), true);
        for (String unsafe : new String[]{"../escape", "/absolute", "C:/absolute", "a\\b", "a/../../escape"})
            check(base, archive(new String[]{unsafe}, new byte[][]{elf}), false);
        check(base, archive(new String[]{"a", "./a"}, new byte[][]{elf, elf}), false);
        byte[] x86 = arm64(); x86[18] = 62;
        check(base, archive(new String[]{"libvulkan.so"}, new byte[][]{x86}), false);
        check(base, archive(new String[]{"libvulkan.so"}, new byte[][]{new byte[8]}), false);
        check(base, new byte[]{1, 2, 3}, false);
        String[] many = new String[129]; byte[][] data = new byte[129][];
        for (int i = 0; i < many.length; i++) { many[i] = "f" + i; data[i] = new byte[0]; }
        check(base, archive(many, data), false);
        // A tiny compressed archive must not expand past the per-file limit.
        ByteArrayOutputStream bytes = new ByteArrayOutputStream();
        try (ZipOutputStream zip = new ZipOutputStream(bytes)) {
            zip.putNextEntry(new ZipEntry("bomb"));
            byte[] block = new byte[1024 * 1024];
            for (int i = 0; i < 129; i++) zip.write(block);
            zip.closeEntry();
        }
        check(base, bytes.toByteArray(), false);
        System.out.println("12 driver archive checks passed (valid ARM64, unsafe paths, duplicate paths, wrong ABI, truncation, invalid ZIP, entry and expansion limits).");
    }
}

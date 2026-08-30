package io.github.dk64port;

import java.io.*;
import java.nio.*;
import java.nio.charset.StandardCharsets;
import java.nio.file.*;
import java.util.*;
import org.json.*;

/** Validates companion packages without loading untrusted code during import. */
final class NativeMod {
    static final long MAX_LIBRARY = 64L * 1024 * 1024;

    static Set<String> libraries(byte[] manifest) throws IOException, JSONException {
        if (manifest == null) return Collections.emptySet();
        JSONObject json = new JSONObject(new String(manifest, StandardCharsets.UTF_8));
        if (!json.has("native_libraries")) return Collections.emptySet();
        Object value = json.get("native_libraries");
        if (!(value instanceof JSONObject)) throw new IOException("Invalid native_libraries manifest field.");
        JSONObject libraries = (JSONObject)value;
        // Keep the initial bridge self-contained. Runtime export behavior for multiple
        // native libraries and additional library dependency packaging needs separate testing.
        if (libraries.length() > 1) throw new IOException("Android bundles currently support one native bridge per mod.");
        Set<String> names = new LinkedHashSet<>();
        Iterator<String> keys = libraries.keys();
        while (keys.hasNext()) {
            String stem = keys.next();
            if (!stem.matches("[A-Za-z_][A-Za-z0-9_]{0,95}")) throw new IOException("Unsafe native library name.");
            Object exportsValue = libraries.get(stem);
            if (!(exportsValue instanceof JSONArray)) throw new IOException("Native exports must be an array.");
            JSONArray exports = (JSONArray)exportsValue;
            if (exports.length() > 256) throw new IOException("Too many native exports.");
            Set<String> seen = new HashSet<>();
            for (int i = 0; i < exports.length(); i++) {
                Object name = exports.get(i);
                if (!(name instanceof String) || !((String)name).matches("[A-Za-z_][A-Za-z0-9_]{0,127}")
                        || !seen.add((String)name)) throw new IOException("Invalid or duplicate native export.");
            }
            names.add(stem + ".so");
        }
        return Collections.unmodifiableSet(names);
    }

    static void validate(Path library) throws IOException {
        if (!Files.isRegularFile(library, LinkOption.NOFOLLOW_LINKS) || Files.size(library) > MAX_LIBRARY)
            throw new IOException("Invalid or oversized native library.");
        DriverArchive.validateArm64(library);
        try (RandomAccessFile input = new RandomAccessFile(library.toFile(), "r")) {
            byte[] bytes = new byte[64];
            input.readFully(bytes);
            ByteBuffer header = ByteBuffer.wrap(bytes).order(ByteOrder.LITTLE_ENDIAN);
            long offset = header.getLong(32);
            int entrySize = Short.toUnsignedInt(header.getShort(54));
            int count = Short.toUnsignedInt(header.getShort(56));
            if (Short.toUnsignedInt(header.getShort(52)) != 64 || entrySize != 56 || count < 1 || count > 1024
                    || offset < 64 || offset > input.length() - (long)entrySize * count)
                throw new IOException("Invalid native ELF program headers.");
            boolean executable = false;
            for (int i = 0; i < count; i++) {
                input.seek(offset + (long)i * entrySize);
                input.readFully(bytes, 0, entrySize);
                ByteBuffer segment = ByteBuffer.wrap(bytes).order(ByteOrder.LITTLE_ENDIAN);
                if (segment.getInt(0) != 1) continue;
                long fileOffset = segment.getLong(8), address = segment.getLong(16);
                long fileSize = segment.getLong(32), memorySize = segment.getLong(40), alignment = segment.getLong(48);
                if (fileOffset < 0 || fileSize < 0 || memorySize < fileSize || fileOffset > input.length() - fileSize
                        || alignment < 16384 || (alignment & (alignment - 1)) != 0
                        || (fileOffset & (alignment - 1)) != (address & (alignment - 1)))
                    throw new IOException("Native bridge needs valid 16 KiB-aligned ARM64 load segments.");
                executable |= (segment.getInt(4) & 1) != 0;
            }
            if (!executable) throw new IOException("Native bridge has no executable load segment.");
        }
        // ELF structure/ABI checks do not establish trust or runtime compatibility.
        // The unchanged runtime validates recomp_api_version and declared exports at launch.
    }
}

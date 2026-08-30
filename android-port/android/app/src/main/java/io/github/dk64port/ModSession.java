package io.github.dk64port;

import java.io.*;
import java.nio.channels.*;
import java.nio.file.*;

/** Cross-process exclusion: never replace mod files or config while the runtime uses them. */
final class ModSession implements AutoCloseable {
    private final FileChannel channel;
    private final FileLock lock;
    private ModSession(FileChannel channel, FileLock lock) { this.channel = channel; this.lock = lock; }
    static ModSession acquire(File filesDirectory) throws IOException {
        FileChannel channel = FileChannel.open(new File(filesDirectory, "mods.lock").toPath(),
            StandardOpenOption.CREATE, StandardOpenOption.WRITE);
        try {
            FileLock lock = null;
            // A manager opened by the exiting game may run just before killProcess releases its lease.
            for (int attempt = 0; attempt < 10; attempt++) {
                lock = channel.tryLock();
                if (lock != null) break;
                try { Thread.sleep(25); }
                catch (InterruptedException error) { Thread.currentThread().interrupt(); throw new IOException("Mod operation interrupted.", error); }
            }
            if (lock == null) throw new IOException("Close the game before managing mods. From the game menu choose Manage Mods.");
            return new ModSession(channel, lock);
        } catch (IOException | OverlappingFileLockException error) {
            channel.close();
            throw new IOException("The game or another mod operation is active. Close it and try again.", error);
        }
    }
    @Override public void close() throws IOException {
        try { lock.release(); } finally { channel.close(); }
    }
}

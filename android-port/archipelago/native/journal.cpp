// SPDX-License-Identifier: GPL-3.0-or-later
#include "state.hpp"
#include <fstream>
#ifdef _WIN32
#define NOMINMAX
#include <windows.h>
#else
#include <fcntl.h>
#include <sys/file.h>
#include <unistd.h>
#include <cerrno>
#endif

namespace dkap {
struct Journal::Impl {
    std::filesystem::path path, temporary;
#ifdef _WIN32
    HANDLE lock = INVALID_HANDLE_VALUE;
#else
    int lock = -1;
#endif
};
Journal::Journal(const std::filesystem::path& path) : impl(std::make_unique<Impl>()) {
    impl->path = path; impl->temporary = path; impl->temporary += ".tmp";
    auto lock_path = path; lock_path += ".lock";
    if (!path.is_absolute() || !std::filesystem::is_directory(path.parent_path())) throw Failure(Error::storage);
#ifdef _WIN32
    impl->lock = CreateFileW(lock_path.c_str(), GENERIC_READ | GENERIC_WRITE, 0, nullptr, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, nullptr);
    if (impl->lock == INVALID_HANDLE_VALUE) throw Failure(Error::storage);
#else
    impl->lock = ::open(lock_path.c_str(), O_CREAT | O_RDWR | O_CLOEXEC | O_NOFOLLOW, 0600);
    if (impl->lock < 0) throw Failure(Error::storage);
    if (flock(impl->lock, LOCK_EX | LOCK_NB)) { close(impl->lock); impl->lock = -1; throw Failure(Error::storage); }
#endif
}
Journal::~Journal() {
#ifdef _WIN32
    if (impl->lock != INVALID_HANDLE_VALUE) CloseHandle(impl->lock);
#else
    if (impl->lock >= 0) close(impl->lock);
#endif
}
Json Journal::load() {
    try {
        if (!std::filesystem::exists(impl->path)) return nullptr;
        if (std::filesystem::is_symlink(impl->path) || !std::filesystem::is_regular_file(impl->path)
            || std::filesystem::file_size(impl->path) > message_limit) throw Failure(Error::storage);
        std::ifstream file(impl->path, std::ios::binary);
        if (!file) throw Failure(Error::storage);
        std::string bytes(message_limit + 1, '\0');
        file.read(bytes.data(), std::streamsize(bytes.size()));
        bytes.resize(size_t(file.gcount()));
        if (file.bad()) throw Failure(Error::storage);
        return parse_bounded(bytes);
    } catch (...) { throw Failure(Error::storage); }
}
void Journal::commit(const Json& data) {
    auto bytes = data.dump();
    if (bytes.size() > message_limit) throw Failure(Error::storage);
#ifdef _WIN32
    HANDLE file = CreateFileW(impl->temporary.c_str(), GENERIC_WRITE, 0, nullptr, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, nullptr);
    if (file == INVALID_HANDLE_VALUE) throw Failure(Error::storage);
    DWORD written = 0;
    bool okay = WriteFile(file, bytes.data(), DWORD(bytes.size()), &written, nullptr) && written == bytes.size() && FlushFileBuffers(file);
    CloseHandle(file);
    if (!okay || !MoveFileExW(impl->temporary.c_str(), impl->path.c_str(), MOVEFILE_REPLACE_EXISTING | MOVEFILE_WRITE_THROUGH))
        throw Failure(Error::storage);
#else
    int file = ::open(impl->temporary.c_str(), O_CREAT | O_TRUNC | O_WRONLY | O_CLOEXEC | O_NOFOLLOW, 0600);
    if (file < 0) throw Failure(Error::storage);
    size_t offset = 0;
    while (offset < bytes.size()) {
        auto amount = ::write(file, bytes.data() + offset, bytes.size() - offset);
        if (amount < 0 && errno == EINTR) continue;
        if (amount <= 0) { close(file); throw Failure(Error::storage); }
        offset += size_t(amount);
    }
    int synced = fsync(file), closed = close(file);
    if (synced || closed || ::rename(impl->temporary.c_str(), impl->path.c_str())) throw Failure(Error::storage);
    int directory = ::open(impl->path.parent_path().c_str(), O_RDONLY | O_CLOEXEC | O_DIRECTORY);
    if (directory < 0) throw Failure(Error::storage);
    synced = fsync(directory); close(directory);
    if (synced) throw Failure(Error::storage);
#endif
}
}

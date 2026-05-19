# Phase 2: Data Models & Local Storage (SQLite) - Detailed Plan

> Define core data structures and persist them in **SQLite** via Qt5::Sql.

**Database:** SQLite (single file, zero-config, embedded)
- Driver: `QSQLITE` (built into Qt5::Sql)
- File: `QStandardPaths::writableLocation(AppDataLocation)/resonate.db`
- No server process needed, data lives in a single `.db` file

## Current State

- `Qt5::Sql` is linked in CMakeLists.txt but unused
- No data model classes exist
- No database schema
- Sidebar navigation items are hardcoded strings
- No persistence of any kind (no QSettings usage)

## Step 1: Define Data Structures

### `include/models/song.hpp`

```cpp
#pragma once
#include <QDateTime>
#include <QString>

struct Song {
    int id = -1;
    QString title;
    QString artist;
    QString album;
    QString filePath;
    int durationMs = 0;
    qint64 fileSize = 0;
    QString format;        // "mp3", "flac", etc.
    QDateTime addedAt;
    QDateTime lastPlayedAt;
    int playCount = 0;
    bool isLiked = false;
};
```

### `include/models/playlist.hpp`

```cpp
#pragma once
#include <QDateTime>
#include <QString>

struct Playlist {
    int id = -1;
    QString name;
    QString description;
    QString coverPath;
    QDateTime createdAt;
    int songCount = 0;
};
```

### `include/models/playlist_song.hpp`

```cpp
#pragma once

struct PlaylistSong {
    int playlistId;
    int songId;
    int position = 0;
};
```

## Step 2: Database Manager

### `include/db/database_manager.hpp`

```cpp
#pragma once
#include <QSqlDatabase>
#include <QString>

class DatabaseManager {
public:
    static DatabaseManager& instance();

    QSqlDatabase database();
    bool initialize();

    DatabaseManager(const DatabaseManager&) = delete;
    DatabaseManager& operator=(const DatabaseManager&) = delete;

private:
    DatabaseManager();
    ~DatabaseManager();

    void createTables();
    void migrateSchema();

    QSqlDatabase db_;
    static constexpr int CURRENT_SCHEMA_VERSION = 1;
};
```

### `src/db/database_manager.cpp`

Key implementation details:

```cpp
// SQLite 连接方式
DatabaseManager::DatabaseManager() {
    QString dbPath = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation)
                     + "/resonate.db";
    QDir().mkpath(QFileInfo(dbPath).absolutePath());

    db_ = QSqlDatabase::addDatabase("QSQLITE");  // SQLite 驱动
    db_.setDatabaseName(dbPath);

    // 可选: 性能优化 (SQLite 特有的 PRAGMA)
    // db_.exec("PRAGMA journal_mode=WAL");
    // db_.exec("PRAGMA synchronous=NORMAL");
}
```

Schema SQL (SQLite 语法):

```sql
CREATE TABLE IF NOT EXISTS schema_version (
    version INTEGER PRIMARY KEY
);

CREATE TABLE IF NOT EXISTS songs (
    id INTEGER PRIMARY KEY AUTOINCREMENT,
    title TEXT NOT NULL,
    artist TEXT DEFAULT '',
    album TEXT DEFAULT '',
    file_path TEXT NOT NULL UNIQUE,
    duration_ms INTEGER DEFAULT 0,
    file_size INTEGER DEFAULT 0,
    format TEXT DEFAULT '',
    added_at TEXT NOT NULL,
    last_played_at TEXT,
    play_count INTEGER DEFAULT 0,
    is_liked INTEGER DEFAULT 0
);

CREATE TABLE IF NOT EXISTS playlists (
    id INTEGER PRIMARY KEY AUTOINCREMENT,
    name TEXT NOT NULL,
    description TEXT DEFAULT '',
    cover_path TEXT DEFAULT '',
    created_at TEXT NOT NULL
);

CREATE TABLE IF NOT EXISTS playlist_songs (
    playlist_id INTEGER NOT NULL,
    song_id INTEGER NOT NULL,
    position INTEGER DEFAULT 0,
    PRIMARY KEY (playlist_id, song_id),
    FOREIGN KEY (playlist_id) REFERENCES playlists(id) ON DELETE CASCADE,
    FOREIGN KEY (song_id) REFERENCES songs(id) ON DELETE CASCADE
);

CREATE TABLE IF NOT EXISTS play_history (
    id INTEGER PRIMARY KEY AUTOINCREMENT,
    song_id INTEGER NOT NULL,
    played_at TEXT NOT NULL,
    FOREIGN KEY (song_id) REFERENCES songs(id) ON DELETE CASCADE
);

CREATE INDEX IF NOT EXISTS idx_songs_title ON songs(title);
CREATE INDEX IF NOT EXISTS idx_songs_artist ON songs(artist);
CREATE INDEX IF NOT EXISTS idx_songs_album ON songs(album);
CREATE INDEX IF NOT EXISTS idx_play_history_song ON play_history(song_id);
CREATE INDEX IF NOT EXISTS idx_play_history_time ON play_history(played_at);
```

- `migrateSchema()`: Read version from `schema_version`, apply incremental migrations

## Step 3: DAO Classes

### `include/db/song_dao.hpp`

```cpp
#pragma once
#include "models/song.hpp"
#include <QVector>

class SongDao {
public:
    explicit SongDao(QSqlDatabase& db);

    int insert(const Song& song);
    bool update(const Song& song);
    bool remove(int id);
    bool removeByPath(const QString& path);

    Song findById(int id);
    Song findByPath(const QString& path);
    QVector<Song> findAll();
    QVector<Song> search(const QString& query);
    QVector<Song> findLiked();
    QVector<Song> findRecent(int limit = 100);
    QVector<Song> findMostPlayed(int limit = 50);
    QVector<Song> findRecentlyAdded(int limit = 50);

    bool setLiked(int id, bool liked);
    bool incrementPlayCount(int id);
    bool updateLastPlayed(int id);

    int count();
    bool existsByPath(const QString& path);

private:
    QSqlDatabase& db_;
    Song songFromQuery(const QSqlQuery& query);
};
```

### `include/db/playlist_dao.hpp`

```cpp
#pragma once
#include "models/playlist.hpp"
#include "models/song.hpp"
#include <QVector>

class PlaylistDao {
public:
    explicit PlaylistDao(QSqlDatabase& db);

    int insert(const Playlist& playlist);
    bool update(const Playlist& playlist);
    bool remove(int id);

    Playlist findById(int id);
    QVector<Playlist> findAll();

    bool addSong(int playlistId, int songId, int position = -1);
    bool removeSong(int playlistId, int songId);
    bool reorderSongs(int playlistId, const QVector<int>& songIds);
    QVector<Song> songs(int playlistId);
    int songCount(int playlistId);

private:
    QSqlDatabase& db_;
};
```

### `include/db/play_history_dao.hpp`

```cpp
#pragma once
#include "models/song.hpp"
#include <QDateTime>
#include <QVector>

struct PlayRecord {
    int id;
    int songId;
    QDateTime playedAt;
    Song song;
};

class PlayHistoryDao {
public:
    explicit PlayHistoryDao(QSqlDatabase& db);

    void record(int songId);
    QVector<PlayRecord> recent(int limit = 100);
    void clear();

private:
    QSqlDatabase& db_;
};
```

## Step 4: Application Settings

### `include/db/settings.hpp`

```cpp
#pragma once
#include <QSettings>
#include <QString>

class Settings {
public:
    static Settings& instance();

    int lastPlayedSongId() const;
    void setLastPlayedSongId(int id);

    qint64 lastPlayedPosition() const;
    void setLastPlayedPosition(qint64 ms);

    int volume() const;            // 0-100, default 80
    void setVolume(int percent);

    int playMode() const;          // 0=Sequential, 1=Loop, 2=SingleLoop, 3=Shuffle
    void setPlayMode(int mode);

    QByteArray windowGeometry() const;
    void setWindowGeometry(const QByteArray& geometry);

    QString musicDirectory() const;
    void setMusicDirectory(const QString& path);

private:
    Settings();
    QSettings settings_;
};
```

All values stored via `QSettings` (INI format at `QStandardPaths::writableLocation(AppDataLocation)/settings.ini`).

## Step 5: CMakeLists.txt Integration

```cmake
# Add to SOURCES:
src/db/database_manager.cpp
src/db/song_dao.cpp
src/db/playlist_dao.cpp
src/db/play_history_dao.cpp
src/db/settings.cpp

# Add to HEADERS:
include/models/song.hpp
include/models/playlist.hpp
include/models/playlist_song.hpp
include/db/database_manager.hpp
include/db/song_dao.hpp
include/db/playlist_dao.hpp
include/db/play_history_dao.hpp
include/db/settings.hpp
```

## Step 6: Initialize in main.cpp

```cpp
DatabaseManager::instance().initialize();
// Settings used by AudioEngine and MainWindow later
```

## Verification

- [ ] Build succeeds with new files
- [ ] Database file created at expected path on first run
- [ ] All tables created with correct schema
- [ ] SongDao insert/query round-trip works
- [ ] PlaylistDao CRUD works
- [ ] PlayHistoryDao record/query works
- [ ] Settings persist across app restarts

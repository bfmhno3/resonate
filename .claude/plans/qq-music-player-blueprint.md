# Resonate - QQ Music Style Player: Implementation Blueprint

> A Qt5 learning project building a desktop music player modeled after QQ Music.

## Current State Assessment

**Working:**
- Frameless window with rounded corners, drop shadow, native resize (WM_NCHITTEST)
- Mac-style traffic light buttons (close/min/max)
- Sidebar navigation (Recommend, Likes, Recent, Local) with page switching
- Custom ClickableSlider with click-to-seek
- Thread-safe file logger

**Not implemented:**
- Audio playback (Qt5::Multimedia linked but unused)
- Data models (no Song/Playlist structs, Qt5::Sql unused)
- All player bar controls (play/pause, prev, next, volume, mode)
- All content pages (empty shells)
- Playlist management
- Search
- Network features (Qt5::Network unused)
- Icons, images, QSS stylesheets (resources.qrc is empty)

---

## Phase 1: Resource System & Visual Foundation

**Goal:** Replace all placeholder colors with real icons and centralized QSS stylesheets.

**Qt5 Concepts:** Qt Resource System (.qrc), QSS stylesheets, QIcon, QPixmap, QPainter.

### Tasks

1. **Collect/generate SVG icon assets**
   - Sidebar icons: home (recommend), heart (likes), clock (recent), folder (local), plus (add playlist)
   - Player bar icons: play, pause, previous, next, volume (high/low/mute), shuffle, repeat, repeat-one, like (filled/outline), playlist, device, desktop-lyric
   - Header icons: back, forward, refresh, settings, search, user avatar placeholder
   - Traffic light icons (or keep current colored circles)
   - Format: SVG preferred (scalable), PNG fallback for complex graphics

2. **Populate `resources/resources.qrc`**
   ```
   /assets/icons/sidebar/
   /assets/icons/player/
   /assets/icons/header/
   /assets/images/ (cover placeholder, default avatar)
   /assets/fonts/ (if using custom font)
   /assets/styles/ (main.qss)
   ```

3. **Create centralized QSS stylesheet (`resources/styles/main.qss`)**
   - Extract all inline stylesheets from .ui files and .cpp constructors
   - Define widget styling: Sidebar, Header, PlayerBar, MainContent
   - Button states: normal, hover, pressed, disabled
   - ScrollBar custom styling (thin, rounded)
   - QListWidget item styling (rounded selection highlight)
   - QLineEdit search pill style
   - Consistent color palette as QSS variables (via property selectors)

4. **Replace placeholder backgrounds with icon-loaded buttons**
   - Use `QIcon(":/assets/icons/...")` in constructors or .ui files
   - Set icon size via `setIconSize()`
   - Apply QSS globally in `main.cpp` via `QFile + qApp->setStyleSheet()`

5. **SidebarItem icon loading**
   - Wire `set_icon()` to actually load from resource paths
   - Pass icon paths when constructing sidebar nav items

**Files modified:** `resources/resources.qrc`, `resources/styles/main.qss` (new), all `src/gui/*.cpp` for icon loading, `src/main.cpp` for global stylesheet.

---

## Phase 2: Data Models & Local Storage (SQLite)

**Goal:** Define core data structures and persist them in **SQLite** (embedded single-file database, `QSQLITE` driver).

**Qt5 Concepts:** QSqlDatabase("QSQLITE"), QSqlQuery, QVariant, QStandardPaths.

### Tasks

1. **Define data structures (`include/models/`)**
   ```cpp
   // song.hpp
   struct Song {
       int id;
       QString title;
       QString artist;
       QString album;
       QString filePath;
       int duration;       // milliseconds
       qint64 fileSize;
       QString format;     // mp3, flac, wav, etc.
       QDateTime addedAt;
       QDateTime lastPlayedAt;
       int playCount;
       bool isLiked;
   };

   // playlist.hpp
   struct Playlist {
       int id;
       QString name;
       QString description;
       QString coverPath;
       QDateTime createdAt;
       int songCount;
   };

   // playlist_song.hpp (junction table)
   struct PlaylistSong {
       int playlistId;
       int songId;
       int position;
   };
   ```

2. **Database manager (`include/db/database_manager.hpp`, `src/db/database_manager.cpp`)**
   - Meyers' singleton (consistent with existing Logger pattern)
   - Initialize SQLite database at `QStandardPaths::writableLocation(AppDataLocation)/resonate.db`
   - `createTables()` - songs, playlists, playlist_songs, play_history
   - Schema migration support (version table + incremental upgrades)

3. **DAO classes (`include/db/`)**
   - `SongDao` - CRUD for songs, search by title/artist/album, bulk insert
   - `PlaylistDao` - CRUD for playlists, add/remove songs, reorder
   - `PlayHistoryDao` - record plays, query recent, compute play counts

4. **Application settings (`include/db/settings.hpp`)**
   - Wrap QSettings for: last played song, volume level, play mode, window geometry, last directory scan path
   - Persist and restore across sessions

**Files created:** `include/models/song.hpp`, `include/models/playlist.hpp`, `include/db/database_manager.hpp/.cpp`, `include/db/song_dao.hpp/.cpp`, `include/db/playlist_dao.hpp/.cpp`, `include/db/play_history_dao.hpp/.cpp`, `include/db/settings.hpp/.cpp`.

**CMakeLists.txt update:** Add new source files to the target.

---

## Phase 3: Audio Engine

**Goal:** Build a playback engine wrapping QMediaPlayer with play modes and state management.

**Qt5 Concepts:** QMediaPlayer, QMediaPlaylist, signals/slots for state changes, QTimer for position tracking.

### Tasks

1. **AudioEngine class (`include/audio/audio_engine.hpp`, `src/audio/audio_engine.cpp`)**
   ```cpp
   class AudioEngine : public QObject {
       Q_OBJECT
   public:
       enum class PlayMode { Sequential, Loop, SingleLoop, Shuffle };
       enum class State { Stopped, Playing, Paused };

       void play(const Song& song);
       void play();
       void pause();
       void stop();
       void next();
       void previous();

       void seek(qint64 positionMs);
       void setVolume(int percent);  // 0-100
       int volume() const;

       void setPlayMode(PlayMode mode);
       PlayMode playMode() const;

       State state() const;
       qint64 position() const;
       qint64 duration() const;
       const Song& currentSong() const;

       void setQueue(const QVector<Song>& songs, int startIndex = 0);
       void addToQueue(const Song& song);
       void clearQueue();

   signals:
       void stateChanged(State state);
       void positionChanged(qint64 positionMs);
       void durationChanged(qint64 durationMs);
       void songChanged(const Song& song);
       void playModeChanged(PlayMode mode);
       void volumeChanged(int percent);
       void queueFinished();
   };
   ```

2. **Internal implementation**
   - Own a `QMediaPlayer` instance
   - Connect QMediaPlayer::positionChanged, durationChanged, stateChanged, mediaStatusChanged
   - Implement shuffle with `QRandomGenerator` + Fisher-Yates on queue indices
   - Single loop: re-seek to 0 on mediaEnd
   - Loop: wrap around at queue end

3. **Metadata reader (`include/audio/metadata_reader.hpp`, `src/audio/metadata_reader.cpp`)**
   - Use `QMediaPlayer`'s `metaData()` to extract title, artist, album, duration, cover art after loading a file
   - Fallback: parse filename as "artist - title" pattern
   - Extract embedded cover art as QPixmap for album art display

4. **Register in MainWindow**
   - Create single AudioEngine instance in MainWindow (or as a singleton)
   - Wire to PlayerBar signals (Phase 4)

**Files created:** `include/audio/audio_engine.hpp/.cpp`, `include/audio/metadata_reader.hpp/.cpp`.

---

## Phase 4: Player Bar Integration

**Goal:** Connect the PlayerBar UI to the AudioEngine so controls actually work.

**Qt5 Concepts:** Signal/slot connections across widgets, QTimer for smooth slider updates, QMediaPlayer state machine.

### Tasks

1. **Wire PlayerBar to AudioEngine**
   - `btn_play_pause::clicked` -> toggle play/pause on AudioEngine
   - `btn_next::clicked` -> AudioEngine::next()
   - `btn_prev::clicked` -> AudioEngine::previous()
   - `btn_volume::clicked` -> mute toggle
   - Volume slider (add if not present) -> AudioEngine::setVolume()
   - `ClickableSlider::slider_moved` -> AudioEngine::seek()
   - AudioEngine::positionChanged -> update slider position
   - AudioEngine::durationChanged -> set slider range
   - AudioEngine::stateChanged -> update play/pause icon
   - AudioEngine::songChanged -> update song title label, cover art, artist label

2. **Progress bar time labels**
   - Add `label_current_time` and `label_total_time` next to the slider
   - Format as "mm:ss" using `QTime::fromMSecsSinceStartOfDay().toString("mm:ss")`

3. **Volume control**
   - Add volume slider (horizontal, compact) near btn_volume
   - Show/hide on button click (popup or inline)
   - Persist volume level via Settings

4. **Play mode cycling**
   - `btn_play_mode::clicked` -> cycle Sequential -> Loop -> SingleLoop -> Shuffle
   - Update icon per mode
   - Persist mode via Settings

5. **Like button**
   - Toggle song's isLiked in database
   - Update icon (filled heart vs outline)
   - Emit signal for Likes page to refresh

6. **Restore last session**
   - On startup: load last played song, position, volume, play mode from Settings
   - Show song info in PlayerBar without auto-playing

**Files modified:** `include/gui/player_bar.hpp`, `src/gui/player_bar.cpp`, `ui/player_bar.ui`, `include/gui/main_window.hpp`, `src/gui/main_window.cpp`.

---

## Phase 5: Local Music Library

**Goal:** Scan directories for audio files, extract metadata, and populate the library.

**Qt5 Concepts:** QDirIterator, QFile, QThread/QRunnable for background scanning, QFileDialog.

### Tasks

1. **MusicScanner worker (`include/library/music_scanner.hpp`, `src/library/music_scanner.cpp`)**
   - Run in QThread to avoid blocking UI
   - Scan user-selected directory recursively
   - Filter by supported extensions: .mp3, .flac, .wav, .aac, .ogg, .wma, .m4a
   - For each file: use MetadataReader to extract song info
   - Bulk-insert into database via SongDao
   - Emit progress(current, total) and finished(newCount, updatedCount) signals

2. **Local music page UI (`include/gui/pages/local_page.hpp`, `src/gui/pages/local_page.cpp`)**
   - "Scan Directory" button -> QFileDialog::getExistingDirectory
   - Progress bar during scan
   - Display songs in a QTableView or custom list widget
   - Columns: Title, Artist, Album, Duration, Format
   - Sort by column click
   - Double-click to play
   - Right-click context menu: Play, Add to playlist, Remove from library, Open file location

3. **Song list widget component (`include/gui/widgets/song_list_widget.hpp`, `src/gui/widgets/song_list_widget.cpp`)**
   - Reusable component used by Local, Likes, Recent, Playlist pages
   - Custom row painting: album art thumbnail, title, artist, duration, like icon
   - Hover highlight, selection highlight
   - Right-click context menu
   - Signal: `songDoubleClicked(Song)` -> play via AudioEngine
   - Signal: `addToPlaylistRequested(Song)` -> show playlist picker

4. **Database population flow**
   - First launch: prompt user to select music directory
   - Store scan path in Settings for re-scanning
   - "Refresh" button in header triggers re-scan of stored path

**Files created:** `include/library/music_scanner.hpp/.cpp`, `include/gui/pages/local_page.hpp/.cpp`, `include/gui/widgets/song_list_widget.hpp/.cpp`.

**Files modified:** `src/gui/main_content.cpp` (replace empty page_local with LocalPage), `src/gui/header.cpp` (wire refresh button).

---

## Phase 6: Playlist Management

**Goal:** Create, edit, delete playlists and manage their songs.

**Qt5 Concepts:** QListWidget custom items, drag-and-drop reorder, QDialog for input, context menus.

### Tasks

1. **Playlist management in sidebar**
   - "Add Playlist" button -> dialog to enter name/description
   - Sidebar playlist list: show playlist name + song count
   - Right-click: Rename, Delete, Play all
   - Click -> navigate to playlist detail page

2. **Playlist detail page (`include/gui/pages/playlist_page.hpp`, `src/gui/pages/playlist_page.cpp`)**
   - Header: playlist cover image, name, description, song count, total duration
   - "Edit" button -> rename dialog
   - "Delete" button -> confirmation dialog
   - "Play All" button -> queue all songs and play
   - Song list using SongListWidget (reused from Phase 5)
   - Drag-and-drop reorder songs within playlist

3. **Add to playlist flow**
   - From any song list (right-click -> "Add to playlist")
   - Show dialog with playlist list (with "Create new" option)
   - Add song to selected playlist_songs junction table

4. **Remove from playlist**
   - Right-click in playlist detail -> "Remove from playlist"
   - Only removes from junction table, not from library

5. **Sidebar playlist count update**
   - Refresh sidebar playlist list on: playlist created, deleted, song added/removed

**Files created:** `include/gui/pages/playlist_page.hpp/.cpp`.

**Files modified:** `src/gui/sidebar.cpp` (playlist list management), `src/gui/main_content.cpp` (add playlist page to stacked widget), `src/gui/main_window.cpp` (wire signals).

---

## Phase 7: Content Pages Implementation

**Goal:** Fill Recommend, Likes, and Recent pages with actual content.

**Qt5 Concepts:** QScrollArea with dynamic content, card-based layouts, QTimer for recent tracking.

### Tasks

1. **Likes page (`include/gui/pages/likes_page.hpp`, `src/gui/pages/likes_page.cpp`)**
   - Query songs where `isLiked = true` from database
   - Display using SongListWidget
   - Auto-refresh when like status changes
   - Empty state: "No liked songs yet" with icon

2. **Recent page (`include/gui/pages/recent_page.hpp`, `src/gui/pages/recent_page.cpp`)**
   - Query play_history ordered by played_at DESC, limit 100
   - Display using SongListWidget
   - "Clear history" button
   - Empty state: "No recently played songs"

3. **Recommend page (`include/gui/pages/recommend_page.hpp`, `src/gui/pages/recommend_page.cpp`)**
   - For local-only player: show "Recently Added" (songs sorted by addedAt DESC)
   - Show "Most Played" (songs sorted by playCount DESC)
   - Card grid layout: album art cards with title/artist
   - ScrollArea with sections (horizontal card rows)
   - Click card -> play that song/album

4. **Play history recording**
   - When a song finishes playing (or plays >30 seconds), record in play_history table
   - Update song's playCount and lastPlayedAt

5. **Empty state components**
   - Reusable empty state widget: icon + message + optional action button
   - Used across all pages when no data exists

**Files created:** `include/gui/pages/likes_page.hpp/.cpp`, `include/gui/pages/recent_page.hpp/.cpp`, `include/gui/pages/recommend_page.hpp/.cpp`, `include/gui/widgets/card_widget.hpp/.cpp`, `include/gui/widgets/empty_state_widget.hpp/.cpp`.

---

## Phase 8: Search

**Goal:** Implement search across the local music library.

**Qt5 Concepts:** QLineEdit signals, QSortFilterProxyModel or custom filtering, QTimer for debouncing.

### Tasks

1. **Search logic**
   - Connect `line_edit_search::textChanged` to search handler
   - Debounce with QTimer (300ms delay after last keystroke)
   - Query database: match title, artist, or album
   - Or use QSortFilterProxyModel on existing model

2. **Search results display**
   - Navigate MainContent stacked widget to a search results page/overlay
   - Display matching songs using SongListWidget
   - Show result count
   - "No results" empty state

3. **Search navigation**
   - Back button in header returns to previous page
   - Clear search restores previous page
   - Enter key -> immediate search (skip debounce)

4. **Keyboard shortcut**
   - Ctrl+F or / -> focus search field

**Files created:** `include/gui/pages/search_page.hpp/.cpp`.

**Files modified:** `src/gui/header.cpp`, `src/gui/main_content.cpp`, `src/gui/main_window.cpp`.

---

## Phase 9: Network Features (Optional/Advanced)

**Goal:** Fetch online music data via network APIs (learning Qt5::Network).

**Qt5 Concepts:** QNetworkAccessManager, QNetworkReply, JSON parsing with QJsonDocument, async patterns.

### Tasks

1. **Network manager (`include/network/api_client.hpp`, `src/network/api_client.cpp`)**
   - Wrap QNetworkAccessManager
   - Base URL configuration
   - Request/response handling with proper error management
   - Rate limiting

2. **Online search**
   - Search API for songs by keyword
   - Display results with "play" and "download" actions
   - Merge online results with local library

3. **Online playback**
   - Stream audio from URL via QMediaPlayer (supports URLs natively)
   - Buffering indicator
   - Fallback on network error

4. **Lyrics fetching**
   - Search for lyrics by song title + artist
   - Parse LRC format
   - Display in a lyrics panel

5. **Album art fetching**
   - If local file has no embedded art, fetch from online API
   - Cache downloaded images to disk

**Note:** This phase requires a music API. Scope depends on API availability.

**Files created:** `include/network/api_client.hpp/.cpp`, `include/network/models.hpp`.

---

## Phase 10: Polish & Advanced Features

**Goal:** Production-quality UI, animations, keyboard shortcuts, and edge cases.

**Qt5 Concepts:** QPropertyAnimation, QShortcut, QSystemTrayIcon, native event handling.

### Tasks

1. **Animations & transitions**
   - Page transition animations (slide or fade between stacked widget pages)
   - Player bar slide-up animation on first song play
   - Button hover/press scale animations
   - Cover art crossfade on song change

2. **Keyboard shortcuts**
   - Space: play/pause
   - Left/Right: seek +/- 5 seconds
   - Up/Down: volume +/- 5%
   - Ctrl+Left/Right: prev/next track
   - Ctrl+F: focus search
   - Ctrl+O: open file dialog
   - Escape: minimize or clear search

3. **System tray**
   - QSystemTrayIcon with context menu (play/pause, next, prev, show/quit)
   - Minimize to tray on close (optional setting)
   - Tray notification on song change

4. **Drag-and-drop support**
   - Drop audio files onto window -> add to library and play
   - Drop files onto playlist -> add to playlist

5. **Mini player mode**
   - Compact window showing only player bar
   - Toggle from full player view

6. **Desktop lyrics overlay**
   - Separate frameless, transparent, always-on-top window
   - Scroll lyrics synced to playback position (if LRC data available)

7. **Multi-monitor DPI handling**
   - Verify existing window chrome works correctly across DPI changes
   - Test shadow rendering on different scaling factors

8. **Accessibility**
   - Tab order for all interactive elements
   - Screen reader labels (accessibleName/Description)
   - High contrast theme support

---

## Implementation Order & Dependencies

```
Phase 1 (Resources)
    |
    v
Phase 2 (Data Models) -----> Phase 3 (Audio Engine)
    |                              |
    v                              v
Phase 5 (Local Library) <---- Phase 4 (Player Bar)
    |
    v
Phase 6 (Playlists) -----> Phase 7 (Content Pages)
                                |
                                v
                          Phase 8 (Search)
                                |
                                v
                          Phase 9 (Network) [optional]
                                |
                                v
                          Phase 10 (Polish)
```

**Parallel opportunities:**
- Phase 1 and Phase 2 can be done in parallel (no code dependency)
- Phase 3 and Phase 5 can be done in parallel after Phase 2
- Phase 7 sub-pages (Likes, Recent, Recommend) can be done in parallel

---

## File Structure After All Phases

```
resonate/
  include/
    audio/
      audio_engine.hpp
      metadata_reader.hpp
    db/
      database_manager.hpp
      song_dao.hpp
      playlist_dao.hpp
      play_history_dao.hpp
      settings.hpp
    gui/
      clickable_slider.hpp
      header.hpp
      main_content.hpp
      main_window.hpp
      player_bar.hpp
      sidebar.hpp
      sidebar_item.hpp
      pages/
        local_page.hpp
        likes_page.hpp
        recent_page.hpp
        recommend_page.hpp
        playlist_page.hpp
        search_page.hpp
      widgets/
        song_list_widget.hpp
        card_widget.hpp
        empty_state_widget.hpp
    library/
      music_scanner.hpp
    models/
      song.hpp
      playlist.hpp
    network/
      api_client.hpp
    utils/
      logger.hpp
  resources/
    resources.qrc
    styles/
      main.qss
    icons/
      sidebar/
      player/
      header/
    images/
    fonts/
  src/
    audio/
      audio_engine.cpp
      metadata_reader.cpp
    db/
      database_manager.cpp
      song_dao.cpp
      playlist_dao.cpp
      play_history_dao.cpp
      settings.cpp
    gui/
      clickable_slider.cpp
      header.cpp
      main_content.cpp
      main_window.cpp
      player_bar.cpp
      sidebar.cpp
      sidebar_item.cpp
      pages/
        local_page.cpp
        likes_page.cpp
        recent_page.cpp
        recommend_page.cpp
        playlist_page.cpp
        search_page.cpp
      widgets/
        song_list_widget.cpp
        card_widget.cpp
        empty_state_widget.cpp
    library/
      music_scanner.cpp
    network/
      api_client.cpp
    utils/
      logger.cpp
    main.cpp
  ui/
    header.ui
    main_content.ui
    main_window.ui
    player_bar.ui
    sidebar.ui
    sidebar_item.ui
    pages/
      local_page.ui
      playlist_page.ui
    dialogs/
      add_to_playlist_dialog.ui
      scan_directory_dialog.ui
```

---

## Estimated Effort Per Phase

| Phase | Scope | Qt5 Learning Focus | Effort |
|-------|-------|-------------------|--------|
| 1. Resources & QSS | Icons, stylesheets, visual polish | Resource system, QSS | Medium |
| 2. Data Models | Structs, SQLite, DAO pattern | QSqlDatabase, QSqlQuery | Medium |
| 3. Audio Engine | Playback core, play modes | QMediaPlayer, QMediaPlaylist | High |
| 4. Player Bar | UI <-> Engine wiring | Signal/slot across widgets | Medium |
| 5. Local Library | File scanning, metadata | QDirIterator, QThread | High |
| 6. Playlists | CRUD, sidebar integration | QDialog, QListWidget custom | Medium |
| 7. Content Pages | Likes, Recent, Recommend | QScrollArea, dynamic layouts | High |
| 8. Search | Filter, results display | QSortFilterProxyModel | Low |
| 9. Network | API client, streaming | QNetworkAccessManager, JSON | High |
| 10. Polish | Animations, tray, shortcuts | QPropertyAnimation, QShortcut | Medium |

---

## Key Design Decisions

1. **SQLite 作为数据库** - 使用 Qt5 内置的 `QSQLITE` 驱动，单文件嵌入式数据库 (`resonate.db`)，无需安装数据库服务器，零配置，适合桌面应用
2. **Singleton pattern for services** - AudioEngine, DatabaseManager, Settings use Meyers' singleton (consistent with existing Logger)
3. **SongListWidget reuse** - One custom widget used across Local, Likes, Recent, Playlist, Search pages
4. **DAO pattern** - Separate data access from business logic, easy to test
5. **QThread for scanning** - Keep UI responsive during directory traversal
6. **QSS centralization** - One stylesheet file, loaded at startup, no inline styles
7. **SVG icons** - Scalable across DPI, tintable via QPainter for state changes

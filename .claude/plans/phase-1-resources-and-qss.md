# Phase 1: Resource System & Visual Foundation - Detailed Plan

> Replace all placeholder colors with real icons and centralized QSS stylesheets.

## Current State

- All buttons use inline stylesheets with placeholder colors (yellow, red, blue, green)
- `resources/resources.qrc` is empty - no assets registered
- `SidebarItem::set_icon()` exists but never receives icon paths
- Navigation items hardcoded: `{"Recommend", "Likes", "Recent", "Local"}`
- No QSS stylesheet files exist

## Step 1: Gather SVG Icon Assets

Create icon files in `resources/icons/`:

**Sidebar icons (`resources/icons/sidebar/`):**
- `recommend.svg` - home/grid icon
- `likes.svg` - heart outline
- `recent.svg` - clock icon
- `local.svg` - folder/music icon
- `playlist.svg` - list icon
- `add.svg` - plus icon

**Player bar icons (`resources/icons/player/`):**
- `play.svg`, `pause.svg`
- `previous.svg`, `next.svg`
- `volume-high.svg`, `volume-low.svg`, `volume-mute.svg`
- `shuffle.svg`, `repeat.svg`, `repeat-one.svg`
- `like.svg` (outline), `like-filled.svg`
- `playlist.svg` - queue/playlist toggle
- `device.svg` - speaker/device
- `desktop-lyric.svg` - lyrics toggle

**Header icons (`resources/icons/header/`):**
- `back.svg`, `forward.svg`, `refresh.svg`
- `search.svg`
- `settings.svg` - gear icon
- `user.svg` - default avatar

**Images (`resources/images/`):**
- `cover-placeholder.svg` - default album art
- `logo.svg` - app logo

Source: Use a consistent icon set. Options:
- Material Design Icons (Apache 2.0)
- Feather Icons (MIT)
- Tabler Icons (MIT)

## Step 2: Populate resources.qrc

Update `resources/resources.qrc` to register all assets:

```xml
<RCC>
    <qresource prefix="/assets">
        <!-- Icons: Sidebar -->
        <file alias="icons/sidebar/recommend.svg">icons/sidebar/recommend.svg</file>
        <file alias="icons/sidebar/likes.svg">icons/sidebar/likes.svg</file>
        <!-- ... all sidebar icons ... -->

        <!-- Icons: Player -->
        <file alias="icons/player/play.svg">icons/player/play.svg</file>
        <file alias="icons/player/pause.svg">icons/player/pause.svg</file>
        <!-- ... all player icons ... -->

        <!-- Icons: Header -->
        <!-- ... all header icons ... -->

        <!-- Images -->
        <file alias="images/cover-placeholder.svg">images/cover-placeholder.svg</file>
        <file alias="images/logo.svg">images/logo.svg</file>

        <!-- Styles -->
        <file alias="styles/main.qss">styles/main.qss</file>
    </qresource>
</RCC>
```

## Step 3: Create Centralized QSS (`resources/styles/main.qss`)

Extract and consolidate all inline styles. Structure:

```qss
/* === Global === */
QWidget { font-family: "Microsoft YaHei", "Segoe UI", sans-serif; }

/* === Sidebar === */
Sidebar { background: #EAEAEA; }
Sidebar QListWidget { ... }
Sidebar QListWidget::item { ... }
Sidebar QListWidget::item:hover { ... }
Sidebar QListWidget::item:selected { ... }

/* === Header === */
Header { background: #F5F5F7; }
Header QLineEdit#line_edit_search { ... }

/* === Player Bar === */
PlayerBar { background: #F9F9F9; border-top: 1px solid #E5E5E5; }
PlayerBar QPushButton { ... }

/* === Main Content === */
MainContent { background: #FFFFFF; }

/* === Scrollbar === */
QScrollBar:vertical { ... }
QScrollBar::handle:vertical { ... }
```

## Step 4: Load QSS in main.cpp

```cpp
// In main.cpp, after QApplication construction:
QFile styleFile(":/assets/styles/main.qss");
if (styleFile.open(QFile::ReadOnly)) {
    qApp->setStyleSheet(styleFile.readAll());
    styleFile.close();
}
```

## Step 5: Replace Placeholder Styles in Each Widget

### Sidebar (`src/gui/sidebar.cpp`)
- Remove inline `setStyleSheet()` calls for buttons
- Set icons: `btn_close->setIcon(QIcon(":/assets/icons/sidebar/close.svg"))`
- Navigation items with icons:
  ```cpp
  QStringList nav_items = {"Recommend", "Likes", "Recent", "Local"};
  QStringList icon_paths = {
      ":/assets/icons/sidebar/recommend.svg",
      ":/assets/icons/sidebar/likes.svg",
      ":/assets/icons/sidebar/recent.svg",
      ":/assets/icons/sidebar/local.svg"
  };
  ```

### PlayerBar (`src/gui/player_bar.cpp`)
- Remove yellow placeholder backgrounds from all buttons
- Set icons for: play/pause, prev, next, volume, mode, like, playlist, device, desktop-lyric
- Set default cover art: `label_cover->setPixmap(QPixmap(":/assets/images/cover-placeholder.svg"))`

### Header (`src/gui/header.cpp`)
- Set icons for: back, forward, refresh, settings, search
- Set default avatar: `label_avatar->setPixmap(QPixmap(":/assets/icons/header/user.svg"))`

### MainWindow (`src/gui/main_window.cpp`)
- Set app logo in sidebar if applicable

## Step 6: SidebarItem Icon Wiring

In `sidebar.cpp`, pass icon paths when creating items:

```cpp
// Current (broken):
auto* item = new SidebarItem(this);
item->set_name(name);
item->set_count(count * 100);
list_navigation_->addItem(item);

// Fixed:
auto* item = new SidebarItem(this);
item->set_name(name);
item->set_count(count * 100);
item->set_icon(icon_paths[count]);  // now actually loads icon
list_navigation_->addItem(item);
```

## Files Modified

| File | Change |
|------|--------|
| `resources/resources.qrc` | Add all asset entries |
| `resources/styles/main.qss` | New - centralized stylesheet |
| `resources/icons/` | New - all SVG icon files |
| `resources/images/` | New - cover placeholder, logo |
| `src/main.cpp` | Load global QSS |
| `src/gui/sidebar.cpp` | Set icons, remove inline styles |
| `src/gui/player_bar.cpp` | Set icons, remove placeholder colors |
| `src/gui/header.cpp` | Set icons, remove placeholder colors |
| `src/gui/sidebar_item.cpp` | Ensure set_icon() works correctly |

## Verification

- [ ] All buttons display icons instead of colored backgrounds
- [ ] Hover/pressed states work on all buttons
- [ ] Sidebar navigation items show icons
- [ ] Player bar shows placeholder cover art
- [ ] Header shows navigation and settings icons
- [ ] Scrollbar styling is applied globally
- [ ] No inline stylesheets remain (all in main.qss)
- [ ] Build succeeds with `cmake --build --preset debug-build`

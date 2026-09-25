import sys
from pathlib import Path

from PyQt6.QtCore import (
    QEasingCurve,
    QPropertyAnimation,
)
from PyQt6.QtGui import (
    QKeySequence,
    QShortcut,
)
from PyQt6.QtWidgets import (
    QApplication,
    QMainWindow,
)

from src.engine import Engine
from src.spotlight import (
    Spotlight,
    SpotlightOverlay,
)


class MainWindow(QMainWindow):
    def __init__(self):
        super().__init__()

        self.setWindowTitle(
            "Zenith Browser - New Tab"
        )

        self.resize(1200, 800)

        # Browser
        self.engine = Engine(self)

        self.engine.titleChanged.connect(
            self.update_title
        )

        self.setCentralWidget(
            self.engine
        )

        # Gray overlay.
        self.overlay = SpotlightOverlay(
            self.engine
        )

        # Spotlight.
        self.spotlight = Spotlight(
            self.engine,
            self
        )

        # Alt + T
        self.shortcut = QShortcut(
            QKeySequence("Alt+T"),
            self
        )

        self.shortcut.activated.connect(
            self.toggle_spotlight
        )

        self.overlay_animation = None

    def toggle_spotlight(self):
        # Prevent Alt+T from spamming/restarting
        # animations.
        if (
            self.spotlight.is_open
            or self.spotlight.is_closing
        ):
            return

        self.open_spotlight()

    def open_spotlight(self):
        # Cover WebEngine.
        self.overlay.setGeometry(
            self.engine.rect()
        )

        # Start completely transparent.
        self.overlay.set_opacity(0.0)

        self.overlay.show()
        self.overlay.raise_()

        # Spotlight above overlay.
        self.position_spotlight()

        self.spotlight.open()
        self.spotlight.raise_()

        # Fade gray overlay in.
        self.overlay_animation = (
            QPropertyAnimation(
                self.overlay,
                b"opacity",
                self
            )
        )

        self.overlay_animation.setDuration(
            250
        )

        self.overlay_animation.setStartValue(
            0.0
        )

        self.overlay_animation.setEndValue(
            1.0
        )

        self.overlay_animation.setEasingCurve(
            QEasingCurve.Type.OutCubic
        )

        self.overlay_animation.start()

    def close_spotlight(self):
        if not self.spotlight.is_open:
            return

        # Close Spotlight first.
        self.spotlight.close()

        # Fade gray overlay out.
        self.overlay_animation = (
            QPropertyAnimation(
                self.overlay,
                b"opacity",
                self
            )
        )

        self.overlay_animation.setDuration(
            220
        )

        self.overlay_animation.setStartValue(
            self.overlay.opacity
        )

        self.overlay_animation.setEndValue(
            0.0
        )

        self.overlay_animation.setEasingCurve(
            QEasingCurve.Type.OutCubic
        )

        self.overlay_animation.finished.connect(
            self._finish_overlay_close
        )

        self.overlay_animation.start()

    def _finish_overlay_close(self):
        self.overlay.hide()
        self.overlay.set_opacity(0.0)

    def position_spotlight(self):
        width = min(
            720,
            self.width() - 80
        )

        self.spotlight.setFixedWidth(
            width
        )

        x = (
            self.width() - width
        ) // 2

        self.spotlight.move(
            x,
            35
        )

    def resizeEvent(self, event):
        super().resizeEvent(event)

        self.overlay.setGeometry(
            self.engine.rect()
        )

        self.position_spotlight()

    def update_title(self, title):
        title = title.strip()

        if title:
            self.setWindowTitle(
                f"Zenith Browser - {title}"
            )
        else:
            self.setWindowTitle(
                "Zenith Browser - New Tab"
            )


def load_theme(app):
    theme_path = (
        Path(__file__).parent / "theme.css"
    )

    with open(
        theme_path,
        "r",
        encoding="utf-8"
    ) as file:
        app.setStyleSheet(
            file.read()
        )


if __name__ == "__main__":
    app = QApplication(sys.argv)

    load_theme(app)

    window = MainWindow()
    window.showMaximized()

    sys.exit(app.exec())
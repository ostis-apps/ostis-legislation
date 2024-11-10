import threading

class ShutdownManager:
    def __init__(self):
        self._stop_event = threading.Event()

    def stop(self):
        self._stop_event.set()

    def is_stopped(self):
        return self._stop_event.is_set()

shutdown_manager = ShutdownManager()
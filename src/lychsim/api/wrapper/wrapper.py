from ..api import Client
from .camera_mixin import CameraCommandsMixin
from .object_mixin import ObjectCommandsMixin


class LychSim(CameraCommandsMixin, ObjectCommandsMixin):
    """LychSim API Wrapper."""

    def __init__(
        self,
        server_name: str = "localhost",
        port: int = 9000,
        width: int = 640,
        height: int = 480,
    ) -> None:
        self.server_name = server_name
        self.port = port
        self.width = width
        self.height = height

        self.client = Client((server_name, port))
        self.client.connect()

        self.post_init()

    def post_init(self) -> None:
        self.warmup_cameras = set()
        self.client.request(f"lych cam set_film_size 0 {self.width} {self.height}")

    def get_status(self) -> str:
        return self.client.request("vget /unrealcv/status")

    def close(self) -> None:
        self.client.disconnect()

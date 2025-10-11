import io
import json

import numpy as np
from PIL import Image


class CameraCommandsMixin:
    """Mixin for camera-related commands."""

    def get_cam_lit(self, cam_id: int) -> Image.Image:
        res = self.client.request(f"lych cam get_lit {cam_id} png")
        return Image.open(io.BytesIO(res))

    def warmup_cam(self, cam_id: int, num_steps: int = 10) -> None:
        for _ in range(num_steps):
            self.client.request(f"lych cam warmup {cam_id}")

    def get_cam_seg(self, cam_id: int) -> Image.Image:
        res = self.client.request(f"lych cam get_seg {cam_id} png")
        res = self.client.request(f"lych cam get_seg {cam_id} png")
        return Image.open(io.BytesIO(res))

    def get_cam_normal(self, cam_id: int) -> Image.Image:
        res = self.client.request(f"lych cam get_normal {cam_id} png")
        res = self.client.request(f"lych cam get_normal {cam_id} png")
        normal = np.array(Image.open(io.BytesIO(res)))[:, :, :3]
        return Image.fromarray(normal)

    def get_cam_depth(self, cam_id: int) -> np.ndarray:
        res = self.client.request(f"lych cam get_depth {cam_id} npy")
        try:
            depth = np.load(io.BytesIO(res))
            return depth
        except Exception:
            raise ValueError(f"Failed to get depth for camera {cam_id}: {res}")

    def get_cam_loc(self, cam_id: int) -> list:
        """Get camera location in world space.
        Args:
            cam_id (int): Camera ID.
        Returns:
            list: Location as [x, y, z] in world space.
        """
        res = self.client.request(f"lych cam get_loc {cam_id}")
        try:
            loc = list(map(float, res.split()))
            return loc
        except Exception:
            raise ValueError(f"Failed to get location for camera {cam_id}: {res}")

    def get_cam_rot(self, cam_id: int) -> list:
        """Get camera rotation in world space.
        Args:
            cam_id (int): Camera ID.
        Returns:
            list: Rotation as [pitch, yaw, roll] in degrees.
        """
        res = self.client.request(f"lych cam get_rot {cam_id}")
        try:
            rot = list(map(float, res.split()))
            return rot
        except Exception:
            raise ValueError(f"Failed to get rotation for camera {cam_id}: {res}")

    def set_cam_loc(self, cam_id: int, loc: list | np.ndarray) -> None:
        """Set camera location in world space.
        Args:
            cam_id (int): Camera ID.
            loc (list | np.ndarray): Location as [x, y, z].
        """
        if isinstance(loc, np.ndarray):
            loc = loc.tolist()
        loc_str = " ".join(map(str, loc))
        self.client.request(f"lych cam set_loc {cam_id} {loc_str}")

    def set_cam_rot(self, cam_id: int, rot: list | np.ndarray) -> None:
        """Set camera rotation in world space.
        Args:
            cam_id (int): Camera ID.
            rot (list | np.ndarray): Rotation as [pitch, yaw, roll] in degrees.
        """
        if isinstance(rot, np.ndarray):
            rot = rot.tolist()
        rot_str = " ".join(map(str, rot))
        self.client.request(f"lych cam set_rot {cam_id} {rot_str}")

    def get_cam_c2w(self, cam_id: int) -> np.ndarray:
        """Get camera-to-world transformation matrix.
        Args:
            cam_id (int): Camera ID.
        Returns:
            np.ndarray: Camera-to-world transformation matrix.
        """
        res = self.client.request(f"lych cam get_c2w {cam_id}")
        res = [float(x) for x in res.strip().split(" ")]
        return np.array(res).reshape(4, 4)

    def get_cam_fov(self, cam_id: int) -> float:
        """Get camera horizontal field of view in degrees.
        Args:
            cam_id (int): Camera ID.
        Returns:
            float: Horizontal field of view in degrees.
        """
        res = self.client.request(f"lych cam get_fov {cam_id}")
        return float(res)

    def get_cam_annots(self, cam_id: int) -> dict:
        res = self.client.request(f"lych cam get_annots {cam_id}")
        return json.loads(res)

    def clear_annot_comps(self) -> None:
        self.client.request("lych cam clear_annot_comps")

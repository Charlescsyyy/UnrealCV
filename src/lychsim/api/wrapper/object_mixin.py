import json
import re

import numpy as np


class ObjectCommandsMixin:
    """Mixin for object-related commands."""

    def get_obj_list(self) -> list[str]:
        res = self.client.request("lych obj list")
        return res.strip().split(" ")

    def get_obj_aabb(self, obj_id: str = None) -> tuple[np.ndarray, np.ndarray]:
        if obj_id is None:
            res = self.client.request("lych obj get_aabb -all")
        else:
            res = self.client.request(f"lych obj get_aabb {obj_id}")
        return json.loads(res)

    def get_obj_obb(self, obj_id: str) -> tuple[np.ndarray, np.ndarray]:
        res = self.client.request(f"lych obj get_obb {obj_id}")
        res = [float(x) for x in res.strip().split(" ")]
        return np.array(res[:3]), np.array(res[3:])

    def get_obj_mask(
        self, cam_id: int, obj_id: str | list[str] = None
    ) -> tuple[list[str], np.ndarray]:
        """Get object mask(s) from a specific camera.

        Arguments:
            cam_id: Camera ID.
            obj_id: If specified, get the mask for this object. If a list of object IDs
                is provided, get the masks for these objects. If None, get the mask for
                all visible objects.

        Returns:
            A list of object masks.
        """
        res = self.client.request("lych object list")
        all_object_ids = res.strip().split(" ")

        object_mask = np.array(self.get_cam_seg(cam_id))

        regexp = re.compile(r"\(R=(.*),G=(.*),B=(.*),A=(.*)\)")

        vis_objects = []
        for obj_id in all_object_ids:
            color_str = self.client.request(f"vget /object/{obj_id}/color")
            match = regexp.match(color_str)
            if not match:
                raise ValueError(f"Invalid color string: {color_str}")
            r, g, b, _ = [int(match.group(i)) for i in range(1, 5)]

            mask = np.all(object_mask[:, :, :3] == [r, g, b], axis=-1)

            area = np.sum(mask)
            if area > 0:
                vis_objects.append((area, obj_id, mask))

        vis_objects.sort(reverse=True)
        vis_masks = np.stack([mask for _, _, mask in vis_objects], axis=0)
        return [obj_id for _, obj_id, _ in vis_objects], vis_masks

    def list_selected(self) -> dict:
        res = self.client.request("lych obj list_selected")
        return json.loads(res)

    def get_obj_annots(self) -> dict:
        res = self.client.request("lych obj get_annots -all")
        try:
            return json.loads(res)
        except Exception:
            raise ValueError(f"Failed to parse JSON: {res}")

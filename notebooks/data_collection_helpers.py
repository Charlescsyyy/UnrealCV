import json
import os
from typing import Any

import numpy as np

from lychsim.api import LychSim


class EasyDict(dict):
    """Convenience class that behaves like a dict but allows access with the attribute syntax."""

    def __getattr__(self, name: str) -> Any:
        try:
            return self[name]
        except KeyError:
            raise AttributeError(name)

    def __setattr__(self, name: str, value: Any) -> None:
        self[name] = value

    def __delattr__(self, name: str) -> None:
        del self[name]


def add_selection_as_floor(state, num_objects):
    objects = state.sim.list_selected()
    if objects["status"] != "ok":
        raise RuntimeError(f"Failed to get selected objects. Response: {objects}")

    new_floors = []
    for obj in objects["outputs"]:
        obj_id = obj["object_id"]
        new_floors.append((obj_id, num_objects))

    before_count = len(state.floors)
    state.floors.update(new_floors)

    print(
        f"Added {len(new_floors)} object(s) to the floor list (prev={before_count} "
        f"-> now={len(state.floors)}):\n{state.floors}"
    )


def add_selection_as_table(state, num_objects):
    objects = state.sim.list_selected()
    if objects["status"] != "ok":
        raise RuntimeError(f"Failed to get selected objects. Response: {objects}")

    new_tables = []
    for obj in objects["outputs"]:
        obj_id = obj["object_id"]
        new_tables.append((obj_id, num_objects))

    before_count = len(state.tables)
    state.tables.update(new_tables)

    print(
        f"Added {len(new_tables)} object(s) to the table list (prev={before_count} "
        f"-> now={len(state.tables)}):\n{state.tables}"
    )


def add_camera_location(state):
    cam_id = state.cam_id
    loc = state.sim.get_cam_loc(cam_id)

    before_count = len(state.cam_locations)
    state.cam_locations.append(loc)

    print(f"New location added (prev={before_count} -> {len(state.cam_locations)}):")
    for loc in state.cam_locations:
        print(f"\t{loc}")


def sample_random_placement(state):
    pass


def get_random_camera_rotations(state):
    def sample_rotation():
        pitch = float(np.random.uniform(state.min_pitch, state.max_pitch))
        yaw = float(np.random.uniform(0, 360))
        roll = 0.0
        return [pitch, yaw, roll]

    return [sample_rotation() for _ in range(state.random_viewpoints_per_location)]


def add_random_camera_height_offset(loc, state):
    offset = float(
        np.random.uniform(
            -state.random_camera_height_offset, state.random_camera_height_offset
        )
    )
    new_loc = loc.copy()
    new_loc[2] += offset
    return new_loc


def set_camera_location_and_rotation(scene_state, cam_loc_final, cam_rot):
    cam_id = scene_state.cam_id
    sim = scene_state.sim

    sim.set_cam_loc(cam_id, cam_loc_final)
    sim.set_cam_rot(cam_id, cam_rot)


def save_state(scene_state):
    save_state = {}
    for k in scene_state:
        if isinstance(scene_state[k], LychSim):
            save_state[k] = str(type(scene_state[k]))
        elif isinstance(scene_state[k], set):
            save_state[k] = list(scene_state[k])
        else:
            save_state[k] = scene_state[k]

    save_path = os.path.join(scene_state.save_path, scene_state.scene_name)
    os.makedirs(save_path, exist_ok=True)

    with open(os.path.join(save_path, "state.json"), "w") as f:
        json.dump(save_state, f, indent=4)


def capture_and_save(scene_state, view_name, camera_warmup_steps=10):
    scene_output_path = os.path.join(
        scene_state.save_path, scene_state.scene_name, view_name
    )
    os.makedirs(scene_output_path, exist_ok=True)

    scene_state.sim.warmup_cam(scene_state.cam_id, camera_warmup_steps)
    image = scene_state.sim.get_cam_lit(scene_state.cam_id)
    image.save(os.path.join(scene_output_path, "lit.png"))

    annots_obj = scene_state.sim.get_obj_annots()
    with open(os.path.join(scene_output_path, "object_annots.json"), "w") as f:
        json.dump(annots_obj, f)

    annots_cam = scene_state.sim.get_cam_annots(scene_state.cam_id)
    with open(os.path.join(scene_output_path, "camera_annots.json"), "w") as f:
        json.dump(annots_cam, f)


def warmup_camera(scene_state, cam_id):
    if cam_id not in scene_state.warmup_cameras:
        for _ in range(scene_state.camera_warmup_steps):
            scene_state.sim.get_cam_lit(cam_id)

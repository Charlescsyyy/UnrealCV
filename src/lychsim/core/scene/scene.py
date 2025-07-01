from typing import Any, Dict

import numpy as np
from shapely import Polygon

from lychsim.core.bbox import OBB
from lychsim.core.object import Object
from lychsim.utils import polygon_to_dict, polygon_from_dict

__all__ = ['SemanticScene', 'SemanticLevel', 'SemanticRegion']


class SemanticRegion:
    """SemanticRegion class represents a region in a SemanticLevel, containing
    objects.
    """

    def __init__(
        self, name: str, uid: str = None, objects: list[Object] = None,
        polygon: Polygon = None, obb: OBB = None
    ):
        self.name = name
        self.uid = uid
        self.objects = objects if objects is not None else []
        self.polygon = polygon
        self.obb = obb

    def get_all_objects(self):
        """Returns the list of objects in the region.
        """
        return self.objects

    def to_dict(self):
        """Returns a dictionary representation of the SemanticRegion.
        """
        return dict(
            name=self.name, uid=self.uid,
            polygon=polygon_to_dict(self.polygon), obb=self.obb.to_dict(),
            objects=[obj.to_dict() for obj in self.objects])

    @classmethod
    def from_dict(cls, data_dict: Dict[str, Any]):
        return cls(
            name=data_dict['name'], uid=data_dict['uid'],
            objects=[
                Object.from_dict(obj) for obj in data_dict['objects']
            ],
            polygon=polygon_from_dict(data_dict['polygon']),
            obb=OBB.from_dict(data_dict['obb'])
        )

    def to_str(self, indent=0):
        if len(self.objects) > 0:
            object_str = [' ' * (indent + 8) + f'Object(name={obj.name}, uid={obj.uid})' for obj in self.objects]
            object_str = '\n' + '\n'.join(object_str) + f'\n{" " * indent}    ]\n'
        else:
            object_str = ']\n'
        indent = ' ' * indent
        return (
            f'{indent}SemanticRegion(\n'
            f'{indent}    name={self.name},\n'
            f'{indent}    uid={self.uid},\n'
            f'{indent}    polygon=...,\n'
            f'{indent}    obb=...,\n'
            f'{indent}    objects=['
            f'{object_str}'
            f'{indent})'
        )

    def __repr__(self):
        return self.to_str()

    def __eq__(self, other):
        if not isinstance(other, SemanticRegion):
            return False
        return (self.name == other.name and self.uid == other.uid and
                self.objects == other.objects and
                self.polygon.equals(other.polygon) and
                self.obb == other.obb)


class SemanticLevel:
    """SemanticLevel class represents a level in a SemanticScene, containing
    regions and objects.
    """

    def __init__(
        self, name: str, uid: str = None, regions: list[SemanticRegion] = None
    ):
        self.name = name
        self.uid = uid
        self.regions = regions if regions is not None else []

    def get_all_objects(self):
        """Returns the list of objects in the level.
        """
        return sum([region.get_all_objects() for region in self.regions], [])

    def get_all_regions(self):
        """Returns the list of regions in the level.
        """
        return self.regions

    def to_dict(self):
        """Returns a dictionary representation of the SemanticLevel.
        """
        return dict(
            name=self.name, uid=self.uid,
            regions=[region.to_dict() for region in self.regions])

    @classmethod
    def from_dict(cls, data_dict: Dict[str, Any]):
        return cls(
            name=data_dict['name'], uid=data_dict['uid'],
            regions=[
                SemanticRegion.from_dict(reg) for reg in data_dict['regions']
            ]
        )

    def to_str(self, indent=0):
        if len(self.regions) > 0:
            region_str = [reg.to_str(indent=indent+8) for reg in self.regions]
            region_str = '\n' + '\n'.join(region_str) + f'\n{" " * indent}    ]\n'
        else:
            region_str = ']\n'
        indent = ' ' * indent
        return (
            f'{indent}SemanticLevel(\n'
            f'{indent}    name={self.name},\n'
            f'{indent}    uid={self.uid},\n'
            f'{indent}    regions=['
            f'{region_str}'
            f'{indent})'
        )

    def __repr__(self):
        return self.to_str()

    def __eq__(self, other):
        if not isinstance(other, SemanticLevel):
            return False
        return (self.name == other.name and self.uid == other.uid and
                self.regions == other.regions)


class SemanticScene:
    """SemanticScene class represents a scene as a hierarchy of levels,
    regions, and objects.
    """

    def __init__(
        self, name: str, uid: str = None, levels: list[SemanticLevel] = None
    ):
        self.name = name
        self.uid = uid
        self.levels = levels if levels is not None else []

    def get_all_objects(self):
        """Returns the list of objects in the scene.
        """
        return sum([level.get_all_objects() for level in self.levels], [])

    def get_all_regions(self):
        """Returns the list of regions in the scene.
        """
        return sum([level.get_all_regions() for level in self.levels], [])

    def to_dict(self):
        """Returns a dictionary representation of the SemanticScene.
        """
        return dict(
            name=self.name, uid=self.uid,
            levels=[level.to_dict() for level in self.levels])

    @classmethod
    def from_npz(cls, npz_path: str):
        data_dict = np.load(npz_path, allow_pickle=True)['data'].item()
        return cls.from_dict(data_dict)

    @classmethod
    def from_dict(cls, data_dict: Dict[str, Any]):
        return cls(
            name=data_dict['name'], uid=data_dict['uid'],
            levels=[
                SemanticLevel.from_dict(lvl) for lvl in data_dict['levels']
            ]
        )

    def to_str(self, indent=0):
        if len(self.levels) > 0:
            level_str = [lvl.to_str(indent=indent+8) for lvl in self.levels]
            level_str = '\n' + '\n'.join(level_str) + f'\n{" " * indent}    ]\n'
        else:
            level_str = ']\n'
        indent = ' ' * indent
        return (
            f'{indent}SemanticScene(\n'
            f'{indent}    name={self.name},\n'
            f'{indent}    uid={self.uid},\n'
            f'{indent}    levels=['
            f'{level_str}'
            f'{indent})'
        )

    def __repr__(self):
        return self.to_str()

    def __eq__(self, other):
        if not isinstance(other, SemanticScene):
            return False
        return (self.name == other.name and self.uid == other.uid and
                self.levels == other.levels)

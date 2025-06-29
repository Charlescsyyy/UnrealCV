from shapely import Polygon

from lychsim.core.object import Object

__all__ = ['SemanticScene', 'SemanticLevel', 'SemanticRegion']


class SemanticRegion:
    """SemanticRegion class represents a region in a SemanticLevel, containing
    objects.
    """

    def __init__(
        self, name: str, objects: list[Object] = None,
        polygon: Polygon = None
    ):
        self.name = name
        self.objects = objects if objects is not None else []
        self.polygon = polygon

    def get_all_objects(self):
        """Returns the list of objects in the region.
        """
        return self.objects

    def to_dict(self):
        """Returns a dictionary representation of the SemanticRegion.
        """
        return dict(
            name=self.name,
            objects=[obj.to_dict() for obj in self.objects])


class SemanticLevel:
    """SemanticLevel class represents a level in a SemanticScene, containing
    regions and objects.
    """

    def __init__(self, name: str, regions: list[SemanticRegion] = None):
        self.name = name
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
            name=self.name,
            regions=[region.to_dict() for region in self.regions])


class SemanticScene:
    """SemanticScene class represents a scene as a hierarchy of levels,
    regions, and objects.
    """

    def __init__(self, name: str, levels: list[SemanticLevel] = None):
        self.name = name
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
            name=self.name,
            levels=[level.to_dict() for level in self.levels])

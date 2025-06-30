from .bbox import OBB

__all__ = ['Object']


class Object:
    """LychSim Object.
    """
    def __init__(
        self, name: str = None, uid: str = None, obb: OBB = None
    ):
        self.name = name
        self.uid = uid
        self.obb = obb

    def to_dict(self):
        """Returns a dictionary representation of the Object.
        """
        return dict(
            name=self.name,
            uid=self.uid,
            obb=self.obb.to_dict() if self.obb is not None else None)

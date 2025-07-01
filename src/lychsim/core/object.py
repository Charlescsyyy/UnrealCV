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

    @classmethod
    def from_dict(cls, data_dict: dict):
        return cls(
            name=data_dict['name'], uid=data_dict['uid'],
            obb=OBB.from_dict(data_dict['obb']) if data_dict['obb'] else None)

    def __eq__(self, other):
        if not isinstance(other, Object):
            return False
        return (self.name == other.name and self.uid == other.uid and
                self.obb == other.obb)

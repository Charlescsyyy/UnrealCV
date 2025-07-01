from typing import Any, Dict

import numpy as np
from shapely import Polygon

__all__ = ['polygon_to_dict', 'polygon_from_dict']


def polygon_to_dict(polygon: Polygon) -> dict:
    return dict(
        exterior=np.array(polygon.exterior.coords),
        holes=[np.array(hole.coords) for hole in polygon.interiors])


def polygon_from_dict(data_dict: Dict[str, Any]) -> Polygon:
    return Polygon(
        data_dict['exterior'], [hole for hole in data_dict['holes']])

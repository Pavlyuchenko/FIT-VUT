import math

points = [
    (-5, -9, 2),
    (-6, 7, -6),
    (-9, 9, 1),
    (-6, 1, 3),
    (-6, -7, 1),
    (-4, 1, -6),
    (-3, 1, 6),
    (-1, 9, -6),
    (-2, -5, 4),
    (-8, -1, 9),
]

centroids = [(-5, -9, 2), (-3, 1, 6), (-2, -5, 4)]


def distance(p1, p2):
    # euclidian
    return math.sqrt((p1[0] - p2[0]) ** 2 + (p1[1] - p2[1]) ** 2 + (p1[2] - p2[2]) ** 2)
    # return abs(p1[0] - p2[0]) + abs(p1[1] - p2[1]) + abs(p1[2] - p2[2])


def add(p1, p2):
    return (p1[0] + p2[0], p1[1] + p2[1], p1[2] + p2[2])


def print_nicely(dct, repetitions):
    print("Repetition " + str(repetitions) + ":")
    for k, v in dct.items():
        print(k, end=": ")
        print(v)
        print()


def k_means(points, centroids, repetitions=4):
    clusters = {c: [] for c in centroids}
    for p in points:
        min_distance = float("inf")
        closest_centroid = None
        for c in centroids:
            d = distance(p, c)
            if d < min_distance:
                min_distance = d
                closest_centroid = c
        clusters[closest_centroid].append(p)

    # recalculate centroids
    new_centroids = []
    # enumerate
    for pts in clusters.values():
        n = len(pts)
        suma = (0, 0, 0)

        for p in pts:
            suma = add(suma, p)

        new_centroids.append((suma[0] / n, suma[1] / n, suma[2] / n))

    print_nicely(clusters, repetitions)

    if repetitions > 1:
        return k_means(points, new_centroids, repetitions - 1)

    return clusters


res = k_means(points, centroids)

# print nicely with desc
"""for k, v in res.items():
    print(k, end=": ")
    print(v)
    print()"""

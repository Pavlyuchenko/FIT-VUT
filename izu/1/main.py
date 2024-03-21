# the costs of moving from one node to another
# Z is a wall
map = """9 8 7 7 9 Z 8 8 8 8
9 7 8 7 8 Z 5 7 7 7
8 7 Z Z Z Z 7 5 6 6
9 8 Z 2 9 Z 8 8 9 9
7 9 Z 4 9 3 3 3 3 3
3 3 3 3 3 Z 4 5 Z 9
6 9 9 9 8 Z 2 6 Z 9
7 9 Z Z Z Z Z Z Z 9
9 9 9 9 9 Z 9 9 9 9
9 9 9 9 9 Z 7 6 8 8"""

start_node = (6, 6)
end_node = (3, 3)

# Nodes are written
OPEN = [(start_node, 0, None)]
CLOSE = []

iteration = 0


def UCS(nodes, OPEN, CLOSE):
    global iteration
    iteration += 1

    if iteration >= 20:
        return "TOO MANY ITERATIONS: No path found"

    if len(OPEN) == 0:
        return "FAILURE: No path found"

    # find smallest cost node in OPEN
    smallest_cost = float("inf")
    smallest_cost_node = None
    for node in OPEN:
        if node[1] < smallest_cost:
            smallest_cost = node[1]
            smallest_cost_node = node
        elif node[1] == smallest_cost:
            if node[0][1] < smallest_cost_node[0][1]:
                smallest_cost = node[1]
                smallest_cost_node = node
            elif node[0][1] == smallest_cost_node[0][1]:
                if node[0][0] < smallest_cost_node[0][0]:
                    smallest_cost = node[1]
                    smallest_cost_node = node

    OPEN.remove(smallest_cost_node)
    curr = smallest_cost_node

    # if the current node is the goal node
    if curr[0] == end_node:
        # print path
        path = []
        while curr[2] is not None:
            path.append(curr[0])
            for node in CLOSE:
                if node[0] == curr[2]:
                    curr = node
                    break
        path.append(start_node)
        path.reverse()
        print("\nPath: ", path)
        return "SUCCESS: Path found"

    # add the current node to the CLOSED list
    CLOSE.append(curr)

    # for each of the current node's neighbors
    for neighbor in neighbors(curr, nodes):
        # if the neighbor is in the CLOSED list
        if neighbor[0] in [node[0] for node in CLOSE]:
            continue
        # if the neighbor is not in the OPEN list
        if neighbor[0] not in [node[0] for node in OPEN]:
            OPEN.append(neighbor)
        # if the neighbor is in the OPEN list
        else:
            for node in OPEN:
                if node[0] == neighbor[0]:
                    if node[1] > neighbor[1]:
                        OPEN.remove(node)
                        OPEN.append(neighbor)

    print("\n----\nIteration: ", iteration)

    # print OPEN
    # sort OPEN by sort[0][1], then sort[0][0]
    OPEN.sort(key=lambda x: (x[1]))
    print("OPEN: ", end=" ")
    for node in OPEN:
        print(
            "([%d, %d], %s, [%d, %d])"
            % (node[0][1], node[0][0], node[1], node[2][1], node[2][0]),
            end=", ",
        )

    # print CLOSE
    # sort CLOSE by sort[0][1], then sort[0][0]
    CLOSE.sort(key=lambda x: (x[1]))
    print("\nCLOSE: ", end=" ")
    for node in CLOSE:
        if node[2] is None:
            print(
                "([%d, %d], %s, NULL)" % (node[0][1], node[0][0], node[1]),
                end=", ",
            )
        else:
            print(
                "([%d, %d], %s, [%d, %d])"
                % (node[0][1], node[0][0], node[1], node[2][1], node[2][0]),
                end=", ",
            )

    return UCS(nodes, OPEN, CLOSE)


# Neighbors can be in all 8 directions
def neighbors(node, nodes):
    i, j = node[0]
    neighbors = []
    for x in range(-1, 2):
        for y in range(-1, 2):
            if i + x < 0 or i + x >= len(nodes) or j + y < 0 or j + y >= len(nodes[0]):
                continue
            if x == 0 and y == 0:
                continue
            if nodes[i + x][j + y] == -1:
                continue
            neighbors.append(((i + x, j + y), node[1] + nodes[i + x][j + y], node[0]))
    return neighbors


nodes = []
for i, row in enumerate(map.split("\n")):
    nodes.append([])
    for j, col in enumerate(row.split(" ")):
        if col == "Z":
            nodes[i].append(-1)
        else:
            nodes[i].append(int(col))

print(UCS(nodes, OPEN, CLOSE))

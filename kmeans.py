import sys


# compares dictionaries, used to check if centroids changed
def is_same(dic1, dic2):
    for i in range(K):
        c_curr = dic1[i][0]
        c_next = dic2[i][0]
        for j in range(d):
            if c_curr[j] != c_next[j]:
                return False
    return True


# used to print centroids at the end, 4 digits after point
def print_centroids(centroids):
    for i in range(K):
        data_point = ["{:.4f}".format(x) for x in centroids[i][0]]
        print(*data_point, sep=",")  # * is used to unpack list


# add two rows by coordinates
def add_data_points(x1, x2):
    return [x1[i] + x2[i] for i in range(len(x1))]


# compute distance between two data points (euclidean norm: sum of squares of x_i-x_j)
def distance_data_points(x1, x2):
    distance = 0
    for i in range(len(x1)):
        distance += (x1[i] - x2[i]) ** 2
    return distance


# used to divide k sums by k cluster sizes to get k means (step 4)
def divide_by_scalar(x1, c):
    return [(x1[i] / c) for i in range(len(x1))]


# default value
max_iter = 200

# check that command line arguments are ok
if len(sys.argv) < 2 or len(sys.argv) > 3:
    print("illegal arguments")
    exit()
try:
    K = int(sys.argv[1])
    if len(sys.argv) == 3:
        max_iter = int(sys.argv[2])
except ValueError:
    print("illegal arguments")
    exit()
if K <= 0 or max_iter <= 0:
    print("illegal arguments")
    exit()

# initialize data points list
data_points = []

# read data points from input
# data_points will contain N data points, each data point is a d-sized list
while True:
    try:
        data_points.append([float(point) for point in input().split(',')])
    except EOFError:
        break

d = len(data_points[0])
N = len(data_points)

# check if K is ok
if K >= N:
    print("illegal arguments")
    exit()

iteration = 0

# initialize centroids
centroids_curr = {i: [[0.0 for j in range(d)], 0] for i in range(K)}
centroids_next = {i: [data_points[i], 1] for i in range(K)}  # initialize centroids

while iteration <= max_iter and not is_same(centroids_curr, centroids_next):

    centroids_curr = centroids_next.copy()
    centroids_next = {i: ([0.0 for j in range(d)], 0) for i in range(K)}

    # step 3
    for i in range(N):

        # find index j, data_points[i] is closest to cluster S_j
        min_distance = distance_data_points(data_points[i], centroids_curr[0][0])
        min_index = 0
        for index in range(K):
            curr_distance = distance_data_points(data_points[i], centroids_curr[index][0])
            if curr_distance < min_distance:
                min_distance = curr_distance
                min_index = index

        j = min_index

        # add to sum and increment counter
        centroids_next[j] = [add_data_points(centroids_next[j][0], data_points[i]), centroids_next[j][1] + 1]

    # divide each centroid by scalar (size of S_j)
    for i in range(K):
        centroids_next[i] = [divide_by_scalar(centroids_next[i][0], centroids_next[i][1]), 0]

    iteration += 1

# print output
print_centroids(centroids_next)

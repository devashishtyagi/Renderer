wp = open("voxels6.txt","w")

data1 = [lines for lines in open("voxels4.txt", "r")]
data2 = [lines for lines in open("voxels5.txt", "r")]

i = 0;
j = 1;

while i < len(data1) and j < len(data2):
	for k in xrange(9):
		wp.write(data1[i+k])
	i = i+10;
	wp.write(data2[j])
	j = j+2

wp.close()


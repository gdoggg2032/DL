import numpy as np
import sys



def makeMapping(MapFile, rnnMapFile):
	mapping = {}
	remapping = {}
	rnnmapping = {}
	with open(MapFile, "r") as f:
		for i, line in enumerate(f):
			label = line.strip().split()
			mapping[label[0]] = i
			remapping[i] = label[1]
	with open(rnnMapFile, "r") as rnnF:
		for line in rnnF:
			tmp = line.split()
			rnnmapping[tmp[0]] = tmp[2]
	return mapping, remapping, rnnmapping

def smooth(labels, numbers):
	'''
	First pass
	'''
	assert(len(labels) == len(numbers))
	length = len(labels)
	i = 0
	while i < length - 1:
		if numbers[i] < 3:
			labels.pop(i)
			numbers.pop(i)
			length -= 1
		i += 1
	i = 0
	while i < length - 1:
		if labels[i] == labels[i + 1]:
			print "HEYHEY"
			labels.pop(i)
			length -= 1
		i += 1
	return labels



def trim(y):
	lenY = len(y)

	trimmedLabels = []
	trimmedNumbers = []
	tmpLabel = y[0]
	cnt = 0
	total = 0
	
	for i in xrange(lenY):
		
		if tmpLabel != y[i]:
			trimmedNumbers.append(cnt)
			trimmedLabels.append(tmpLabel)
			tmpLabel = y[i]
			total += cnt
			cnt = 0
		cnt += 1
	trimmedLabels.append(y[-1])
	trimmedNumbers.append(cnt)
	# total += cnt
	# print trimmedLabels
	# print trimmedNumbers
	# print total
	return trimmedLabels, trimmedNumbers



MapFile = "./48_39.map"
rnnMapFile = "./48_idx_chr.map_b"

testFile = open("./try.test.out", "r")
newTestFile = open("./ff.ans1.out", "w")
output = open("./ff.ans1.out.csv", "w")
SeqLengthFile = open("./SeqLength.txt", "r")
mapping, remapping, rnnmapping = makeMapping(MapFile, rnnMapFile)


tmpLine = testFile.readline()
labels = []
sentenceIDs = []
tmpSentenceID = "fadg0_si1279"
newTestFile.write("ID,Prediction\n")
output.write("id,phone_sequence\n")

if sys.argv[1] == 'y':
	index = 0
	seqlength = []
	total = 0
	totalLength = 0 
	for i in SeqLengthFile:
		seqlength.append(int(i))

	assert(len(seqlength) == 592)

	for i, line in enumerate(testFile):
		labels.append(line.strip().split(",")[1])
		if i == total + seqlength[index] - 2:
			#print i
			total += seqlength[index]
			index += 1
			trimmedLabels, trimmedNumbers = trim(labels)
			trimmedLabels = smooth(trimmedLabels, trimmedNumbers)

			tmp = line.strip().split(",")[0].split("_")

			#print tmp[0] + "_" + tmp[1]
			output.write(tmp[0] + "_" + tmp[1] + ",")
			for lbl in trimmedLabels:
				totalLength += 1
				output.write(rnnmapping[lbl])
			output.write("\n")

			labels = []
	print totalLength
	print "average length: ", totalLength / 592.0





else:
	seq_dict = {}
	ID_list = []
	for line in testFile:
		line = line.strip().split(',')
		sag = line[0].split('_')
		seq_id = sag[0]+'_'+sag[1]
		if seq_id not in seq_dict:
			seq_dict[seq_id] = []
			ID_list.append(seq_id)
		if len(seq_dict[seq_id]) > 0:
			if line[1] != seq_dict[seq_id][len(seq_dict[seq_id])-1]:
				seq_dict[seq_id].append(line[1])
		else:
			seq_dict[seq_id].append(line[1])

	for seq in ID_list:
		output.write(seq+',')
		for label in seq_dict[seq]:
			output.write(rnnmapping[label])
		output.write('\n')







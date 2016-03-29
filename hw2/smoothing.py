import numpy as np



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



def trimAndDelete(y):
	lenY = len(y)

	trimmedLabels = []
	trimmedNumbers = []
	tmpLabel = y[0]
	cnt = 0
	
	for i in xrange(lenY):
		
		if tmpLabel != y[i]:
			trimmedNumbers.append(cnt)
			trimmedLabels.append(tmpLabel)
			tmpLabel = y[i]
			cnt = 0
		cnt += 1
	print len(trimmedLabels)

	#------ Replacing ------#
	assert (len(trimmedLabels) == len(trimmedNumbers))
	lenTrimmed = len(trimmedLabels)
	i = 0
	while True:
		#print i
		if i >= lenTrimmed - 1:
			break

		if trimmedNumbers[i] < 5 and trimmedLabels[i - 1] == trimmedLabels[i + 1]:
			trimmedLabels.pop(i)
			trimmedLabels.pop(i)
			trimmedNumbers[i - 1] += trimmedNumbers[i] + trimmedNumbers[i + 1]
			trimmedNumbers.pop(i)
			trimmedNumbers.pop(i)
			lenTrimmed -= 2
			if i == lenTrimmed - 1:
				break
			continue
		i += 1

	
	
	#------ Deleting ------#
	i = 0
	lenTrimmed = len(trimmedNumbers)
	while True:
		if trimmedNumbers[i] < 5:
			trimmedLabels.pop(i)
			trimmedNumbers.pop(i)
			lenTrimmed -= 1
		i += 1
		if i >= lenTrimmed:
			break
	return trimmedLabels

MapFile = "./48_39.map"
rnnMapFile = "./48_idx_chr.map_b"

testFile = open("outputN3_256_mom.csv", "r")
newTestFile = open("./outputN3_256_supersmoothed.csv", "w")
output = open("./output.csv", "w")
mapping, remapping, rnnmapping = makeMapping(MapFile, rnnMapFile)


tmpLine = testFile.readline()
labels = []
sentenceIDs = []
tmpSentenceID = "fadg0_si1279"
newTestFile.write("ID,Prediction\n")
output.write("id,phone_sequence\n")
totalLength = 0
while True:
	tmpLine = testFile.readline().strip()
	if tmpLine == "":
		break
	tmp = tmpLine.split(",")
	S = tmp[0].split("_")
	sentenceID = S[0] + "_" + S[1]
	if sentenceID != tmpSentenceID:
		trimmedLabels = trimAndDelete(labels)
		newTestFile.write(tmpSentenceID + ",sil ")
		output.write(tmpSentenceID + ",L")
		for i in trimmedLabels:
			if remapping[i] != "sil":
				newTestFile.write(remapping[i] + " ")
				output.write(rnnmapping[remapping[i]])
				totalLength += 1
		newTestFile.write("sil\n")
		output.write("L\n")
		labels = []
		tmpSentenceID = sentenceID

	labels.append(mapping[tmp[1]])
trimmedLabels = trimAndDelete(labels)
newTestFile.write(tmpSentenceID + ",sil ")
output.write(tmpSentenceID + ",L")
for i in trimmedLabels:
	if remapping[i] != "sil":
		newTestFile.write(remapping[i] + " ")
		output.write(rnnmapping[remapping[i]])
		totalLength += 1
newTestFile.write("sil\n")
output.write("L\n")
print float(totalLength) / 592.0
	







import cPickle as pickle

def packData(TrainFile, LabelFile, TestFile):

	#---for train file---
	TrainSeq = {}
	Trainlist = []
	#for data part
	with open(TrainFile, "r") as f:
		for line in f:
			ll = line.strip().split(" ")
			name_seqid = ll.pop(0) #name_seqid
			name = "_".join(name_seqid.split("_")[0:len(name_seqid.split("_"))-1])
			seqid = name_seqid.split("_")[-1]
			if name not in TrainSeq:
				TrainSeq[name]={'label':[], 'data':[], 'order':len(TrainSeq)}
				Trainlist.append(TrainSeq[name])
			features = [float(i) for i in ll]
			TrainSeq[name]['data'].append(features)

	#for label part
	with open(LabelFile, "r") as f:
		for line in f:
			ll = line.strip().split(",")
			name_seqid = ll[0]
			name = "_".join(name_seqid.split("_")[0:len(name_seqid.split("_"))-1])
			label = ll[1]
			TrainSeq[name]['label'].append(label)

	#---for test file----
	TestSeq = {}
	Testlist = []
	with open(TestFile, "r") as f:
		for line in f:
			ll = line.strip().split(" ")
			name_seqid = ll.pop(0) #name_seqid
			name = "_".join(name_seqid.split("_")[0:len(name_seqid.split("_"))-1])
			seqid = name_seqid.split("_")[-1]
			if name not in TestSeq:
				TestSeq[name]={'label':[], 'data':[], 'order':len(TestSeq)}
				Testlist.append(TestSeq[name])
			features = [float(i) for i in ll]
			TestSeq[name]['data'].append(features)

	#sorting by order...?
	'''
	for i in xrange(len(Xseq)):
		while True:
			checkseqlabel = Yseq[i][0]
			if checkseqlabel == 'sil':
				Yseq[i].pop(0)
				Xseq[i].pop(0)
			else:
				break
		while True:
			checkseqlabel = Yseq[i][len(Yseq[i])-1]
			if checkseqlabel == 'sil':
				Yseq[i].pop(len(Yseq[i])-1)
				Xseq[i].pop(len(Yseq[i])-1)
			else:
				break'''

	Xseq = []
	Yseq = []
	for n, seq in enumerate(Trainlist):
		#print seq['order']
		data = seq['data']
		Xtmp = [data[i:i+100] for i in xrange(0,len(data),100)]

		label = seq['label']
		Ytmp = [label[i:i+100] for i in xrange(0,len(label),100)]
		print len(Xtmp), len(Ytmp)
		Xseq += Xtmp
		Yseq += Ytmp

	Tseq = []
	for n, seq in enumerate(Testlist):
		#print seq['order']
		data = seq['data']
		Xtmp = [data[i:i+100] for i in xrange(0,len(data),100)]
		print len(Xtmp)
		Tseq += Xtmp



	



	print len(Xseq), len(Xseq[0]),len(Xseq[1]),len(Xseq[3]),len(Xseq[4])
	print len(Yseq), len(Yseq[0])
	return Xseq, Yseq, Tseq



if __name__ == "__main__":

	TrainFile = "./Train_data_sorted.out"
	LabelFile = "./Train_label_sorted.out"
	# LabelFile = "./MLDS/label/train.lab"
	TestFile = "./posteriorgram/test.post"

	Xseq, Yseq, Testseq = packData(TrainFile, LabelFile, TestFile)
	#3 layer list structure
	#1st layer: [seq], each item is a sequence list. total len = number of sequences
	#2st layer: [x], each item is a x in a sequence, total len = size of this sequence
	#3st layer: [features], each item is a feature of this x, total len is the dimension of an x
	print len(Xseq)
	print len(Xseq[0])

	TrainFile_pkl = "./SoftmaxTrain_my_data.pkl"
	LabelFile_pkl = "./SoftmaxTrain_my_lab.pkl"
	TestFile_pkl = "./SoftmaxTest_data.pkl"

	pickle.dump(Xseq, open(TrainFile_pkl, "wb"))
	pickle.dump(Yseq, open(LabelFile_pkl, "wb"))
	pickle.dump(Testseq, open(TestFile_pkl, "wb"))

	XseqTRY = pickle.load(open(TrainFile_pkl, "rb"))
	print len(XseqTRY)
	print len(XseqTRY[0])










import tensorflow as tf
from tensorflow.python.client import timeline
import sys

#############################################
#Toy Program to demonstrate how Tensorflow ##
#data flow graphs are defined and executed ##					
#############################################

#Declare the Tensorflow graph object
toy_graph = tf.Graph()

#Define the graph
with toy_graph.as_default():
	#Hierarchical naming convention
	with tf.name_scope("toy_op"):
		#Declare the variables
		matrix_1 = tf.Variable(tf.random_uniform([4096,4096])
			,name="matrix_1")
		matrix_2 = tf.Variable(tf.random_uniform([4096,4096])
			,name="matrix_2")
		matrix_3 = tf.Variable(tf.random_uniform([4096,4096])
			,name="matrix_3")
		matrix_4 = tf.Variable(tf.random_uniform([4096,4096])
			,name="matrix_4")

		init = tf.initialize_all_variables()

		#Matrix Multiplication
		mat_mul_1_2 = tf.matmul(matrix_1, matrix_2)
		mat_mul_3_4 = tf.matmul(matrix_3, matrix_4)

		add_fin = tf.add(mat_mul_1_2, mat_mul_3_4)

#Create the session object and run it
with tf.Session(graph=toy_graph, config=tf.ConfigProto(allow_soft_placement=True)) as sess:
	#trace options to generate a timeline json
	options = tf.RunOptions(trace_level=tf.RunOptions.FULL_TRACE)
	run_metadata = tf.RunMetadata()
	sess.run(init)
	print("Initialization complete\n")
	sess.run(add_fin, options=options, run_metadata=run_metadata)

	#Create the timeline and write it to a json file
	print "Step Stats:"
	print run_metadata.step_stats
	
	fetched_timeline = timeline.Timeline(run_metadata.step_stats)
	chrome_trace = fetched_timeline.generate_chrome_trace_format()
	with open('timelineLatest.json', 'w') as f:
		f.write(chrome_trace)


#Initialize the session
#sess = tf.Session(graph=toy_graph, config=tf.ConfigProto(
#	log_device_placement=True))

#Log file writer for Tensorboard visualization
#writer = tf.summary.FileWriter(sys.argv[1], sess.graph)

#Run the session

#sess.run(init)					#init variables first
#print(sess.run(add_fin))		#run the add_fin node

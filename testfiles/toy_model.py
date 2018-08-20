import tensorflow as tf
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

		#GPU-0
		with tf.device("/gpu:0"):
			matrix_1 = tf.Variable(tf.random_uniform([4096,4096])
				,name="matrix_1")
			matrix_2 = tf.Variable(tf.random_uniform([4096,4096])
				,name="matrix_2")
		#GPU-1
		with tf.device("/gpu:1"):
			matrix_3 = tf.Variable(tf.random_uniform([4096,4096])
				,name="matrix_3")
			matrix_4 = tf.Variable(tf.random_uniform([4096,4096])
				,name="matrix_4")
		
		init = tf.initialize_all_variables()

		#First Matrix Multiplication
		with tf.device("/gpu:0"):
			mat_mul_1_2 = tf.matmul(matrix_1, matrix_2)

		#Second Matrix Multiplication
		with tf.device("/gpu:1"):
			mat_mul_3_4 = tf.matmul(matrix_3, matrix_4)

		#Final Addition
		add_fin = tf.add(mat_mul_1_2, mat_mul_3_4)

#Initialize the session
sess = tf.Session(graph=toy_graph, config=tf.ConfigProto(
	log_device_placement=True))

#Log file writer for Tensorboard visualization
writer = tf.summary.FileWriter(sys.argv[1], sess.graph)

#Run the session

sess.run(init)					#init variables first
print(sess.run(add_fin))		#run the mat_mul node

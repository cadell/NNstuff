// TestingEigen.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <Eigen/Core>
#include <Eigen/Dense>
#include <random>
#include <vector>
#include <iostream>
#include <numeric>
#include <tuple>
#include <math.h>

using namespace std;
/****************************************************************************************
	Initialize the layers of the current network given an array with the amount of layers
	and nodes
*****************************************************************************************/
std::vector<Eigen::VectorXd> bias_array;
std::vector<Eigen::MatrixXd> weight_matrix;

std::tuple<std::vector<Eigen::MatrixXd>, std::vector<Eigen::VectorXd>> initLayers(std::vector<int> size)
{
	cout.precision(17);
	std::default_random_engine generator;
	std::normal_distribution<double> distribution(1.0, 0.5);
	//int bias_size = std::accumulate(size.begin() + 1, size.end(), 0);
	for (unsigned int i = 1; i < std::size(size); i++)
	{
		Eigen::VectorXd bias(size[i]);
		bias = Eigen::VectorXd::Zero(size[i]);
		for (int j = 0; j < size[i]; j++)
		{
			bias[j] = distribution(generator);
		}
		bias_array.push_back(bias);
		std::cout << "current Bias generated for " << i << "\n" <<bias << std::endl;
	}

	for (unsigned int i = 0; i < std::size(size); i++)
	{
		unsigned int rows = 0;
		unsigned int cols = 0;

		if (i < std::size(size))
		{
			unsigned int next = i + 1;
			if (next >= std::size(size)) break;
			rows = size[next];
			cols = size[i];

			Eigen::MatrixXd weight(rows, cols);
			weight = Eigen::MatrixXd::Zero(rows, cols);

			for (unsigned int r = 0; r < rows; r++)
			{
				for (unsigned int c = 0; c < cols; c++)
				{
					//weight <<  distribution(generator);
					weight(r, c) = distribution(generator);
				}
			}
			weight_matrix.push_back(weight);
			std::cout << "Current weights generated \n" <<weight << std::endl;
		}

	}
	return std::make_tuple(weight_matrix, bias_array); //Order of the tuple matters
}
/****************************************************************************************
	Sigmoid Squishification Functions
*****************************************************************************************/
template <class E> double sigmoid(E layer_activation)
{
	return (1 / (1 + std::exp(-layer_activation)));
}
template <class E> double sigmoidPrime(E layer_activation)
{
	return sigmoid(layer_activation) * (1 - sigmoid(layer_activation));
}
template <class E > E cost_derivative(E training_inputs, E desired_output)
{
	return (training_inputs - desired_output);
}
/****************************************************************************************
	Foward Propagation by calculating the action values of every layer in our network
	This will be used if training data is provided to the network
*****************************************************************************************/ 
std::tuple<std::vector<Eigen::VectorXd>, std::vector<Eigen::VectorXd>> propFoward(Eigen::MatrixXd activ, Eigen::MatrixXd init_weights, Eigen::VectorXd init_bias)
{	
	std::vector<Eigen::VectorXd> activations;
	std::vector<Eigen::VectorXd> unsquished_activations;
	std::cout << init_weights.transpose() << " " <<activ.transpose() <<std::endl;
	Eigen::VectorXd layer_activation = init_weights*activ + init_bias;//activ.dot(init_weights);
	unsquished_activations.push_back(layer_activation);
	for (unsigned int i = 0 ; i < (unsigned)layer_activation.size(); i++)
	{
		layer_activation[i] = sigmoid(layer_activation[i]);
	}
	activations.push_back(layer_activation);
	std::cout <<layer_activation << std::endl;// + init_bias;
	return std::make_tuple(activations,unsquished_activations);
}

/****************************************************************************************
	This weill be depricated we have no need for this
*****************************************************************************************/
std::vector<double> cost(std::vector<Eigen::VectorXd> activations, std::vector<double> labels)
{
	//Eigen::VectorXd cost(activations[activations.size()-1].size()-1);//Grab the last vector which contains the last eigen vector of activation values
	std::vector<double> cost;
	auto cost_activation_vector = activations[activations.size() - 1];
	double sum = 0.0;
	for (int i = 0; i < labels.size(); i++)
	{
		sum += std::pow((cost_activation_vector[i]-labels[i]),2);
	}
	cost.push_back(sum);
	return cost;
}
/****************************************************************************************
	Back_prog
*****************************************************************************************/
template <class B, class E > void backProp(E training_inputs, B desired_output)
{
	auto new_bias = bias_array;
	auto new_weights = weight_matrix;
//	Eigen::MatrixXd new_weights
	std::vector<Eigen::VectorXd> activations;
	std::vector<Eigen::VectorXd> z;
	for (unsigned int i = 0; i < bias_array.size(); i++)
	{
		auto hold = propFoward(training_inputs, weight_matrix[i], bias_array[i]);
		activations = std::get<0>(hold);
		z = std::get<1>(hold);
	}
	Eigen::VectorXd vec = activations[activations.size() - 1];
	auto delta = cost_derivative(vec, desired_output) * sigmoidPrime(z.size() - 1);
	cout << "\n" << endl;
	cout << delta << endl;
	new_bias[new_bias.size() - 1] = delta;
	//new_weights[new_weights.size() -1] = 
}
/****************************************************************************************
Back_prog
*****************************************************************************************/
int main()
{
	int  arr[] = { 1, 1, 1, 1, 1, 1 };
	Eigen::MatrixXd fake_vals;// (6, 1);
	fake_vals = Eigen::MatrixXd::Zero(14, 1);

	Eigen::VectorXd fake_label;// (6, 1);
	fake_label = Eigen::VectorXd::Zero(1);

	std::vector<int> test = { 14,3,4,1};
	auto hold = initLayers(test);
	fake_vals  << 1, 2, 1, 0, 0, 1, 8, 0, 0, 0, 3, 5, 6, 2;
	fake_label << 0;

	propFoward(fake_vals, std::get<0>(hold)[0], std::get<1>(hold)[0]);
	backProp(fake_vals, fake_label);
	std::cin.get();
    return 0;
}


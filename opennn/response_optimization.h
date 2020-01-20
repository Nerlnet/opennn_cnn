//   OpenNN: Open Neural Networks Library
//   www.opennn.net
//
//   R E S P O N S E   O P T I M I Z A T I O N   C L A S S   H E A D E R   
//
//   Artificial Intelligence Techniques SL
//   artelnics@artelnics.com



#ifndef RESPONSEOPTIMIZATION_H
#define RESPONSEOPTIMIZATION_H

// System includes

#include <cmath>
#include <cstdlib>
#include <fstream>
#include <iostream>
#include <string>
#include <sstream>

// OpenNN includes



#include "neural_network.h"



#include "tinyxml2.h"

namespace OpenNN
{

/// This class is used to optimize model response identify the combinations of variable settings jointly optimize a set of responses.

///
/// This tool is adequate when you need to know the behaviour of a multiple variables on a response
/// and satisfy the requirements of the architecture.

class ResponseOptimization
{

public:

   // DEFAULT CONSTRUCTOR

    explicit ResponseOptimization();

    explicit ResponseOptimization(NeuralNetwork*);

    void set_evaluations_number(const int&);

   

   virtual ~ResponseOptimization();

    ///Enumeration of available conditions for response optimization.

   enum Condition{Between, EqualTo, LessEqualTo, GreaterEqualTo, Minimum, Maximum};

   ///
   /// This structure returns the results obtained in the optimization, e.g. optimum inputs number, etc.
   ///

   struct Results
   {
       /// Default constructor.

       explicit Results(NeuralNetwork* new_neural_network_pointer)
       {
           neural_network_pointer = new_neural_network_pointer;
       }

       virtual ~Results(){}

       NeuralNetwork* neural_network_pointer = nullptr;

       Tensor<double, 1> optimal_variables;

       double optimum_objective;

       void print() const
       {
           const int inputs_number = neural_network_pointer->get_inputs_number();
           const int outputs_number = neural_network_pointer->get_outputs_number();

           const vector<string> inputs_names = neural_network_pointer->get_inputs_names();
           const vector<string> outputs_names = neural_network_pointer->get_outputs_names();

           for(int i = 0; i < inputs_number; i++)
           {
               cout << inputs_names[i] << ": " << optimal_variables[i] << endl;
           }

           for(int i = 0; i < outputs_number; i++)
           {
               cout << outputs_names[i] << " " << optimal_variables[inputs_number+i] << endl;
           }

           cout << "Objective: " << optimum_objective << endl;
       }
   };


   // Get methods

   vector<Condition> get_inputs_conditions();
   vector<Condition> get_outputs_conditions();

   Tensor<double, 1> get_inputs_minimums();
   Tensor<double, 1> get_inputs_maximums();
   Tensor<double, 1> get_outputs_minimums();
   Tensor<double, 1> get_outputs_maximums();

   // Set methods

   void set_input_condition(const string&, const Condition&, const Tensor<double, 1>& = Tensor<double, 1>());
   void set_output_condition(const string&, const Condition&, const Tensor<double, 1>& = Tensor<double, 1>());

   void set_input_condition(const int&, const Condition&, const Tensor<double, 1>& = Tensor<double, 1>());
   void set_output_condition(const int&, const Condition&, const Tensor<double, 1>& = Tensor<double, 1>());

   void set_inputs_outputs_conditions(const vector<string>&, const vector<string>&, const Tensor<double, 1>& = Tensor<double, 1>());

   vector<Condition> get_conditions(const vector<string>&) const;
   vector<Tensor<double, 1>> get_values_conditions(const vector<Condition>&, const Tensor<double, 1>&) const;

   Tensor<double, 2> calculate_inputs() const;

   Tensor<double, 2> calculate_envelope(const Tensor<double, 2>&, const Tensor<double, 2>&) const;

   Results* perform_optimization() const;

private:

    NeuralNetwork* neural_network_pointer = nullptr;

    vector<Condition> inputs_conditions;
    vector<Condition> outputs_conditions;

    Tensor<double, 1> inputs_minimums;
    Tensor<double, 1> inputs_maximums;

    Tensor<double, 1> outputs_minimums;
    Tensor<double, 1> outputs_maximums;

    int evaluations_number = 1000;

    double calculate_random_uniform(const double&, const double&) const;

};

}

#endif


// OpenNN: Open Neural Networks Library.
// Copyright(C) 2005-2020 Artificial Intelligence Techniques, SL.
//
// This library is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public
// License as published by the Free Software Foundation; either
// version 2.1 of the License, or any later version.
//
// This library is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// Lesser General Public License for more details.

// You should have received a copy of the GNU Lesser General Public
// License along with this library; if not, write to the Free Software

// Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA


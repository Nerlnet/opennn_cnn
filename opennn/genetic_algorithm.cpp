//   OpenNN: Open Neural Networks Library
//   www.opennn.net
//
//   G E N E T I C   A L G O R I T H M   C L A S S
//
//   Artificial Intelligence Techniques SL
//   artelnics@artelnics.com

#include "genetic_algorithm.h"

namespace opennn
{

    /// Default constructor.

    GeneticAlgorithm::GeneticAlgorithm()
        : InputsSelection()
    {
        set_default();
    }


    /// Training strategy constructor.
    /// @param new_training_strategy_pointer Pointer to a training strategy object.

    GeneticAlgorithm::GeneticAlgorithm(TrainingStrategy* new_training_strategy_pointer)
        : InputsSelection(new_training_strategy_pointer)
    {
        set_default();
    }

    /// Returns the population matrix.

    const Tensor<bool, 2>& GeneticAlgorithm::get_population() const
    {
        return population;
    }


    /// Returns the fitness of the population.

    const Tensor<type, 1>& GeneticAlgorithm::get_fitness() const
    {
        return fitness;
    }


    const Tensor<bool, 1>& GeneticAlgorithm::get_selection() const
    {
        return selection;
    }


    /// Returns the size of the population.

    Index GeneticAlgorithm::get_individuals_number() const
    {
        return population.dimension(0);
    }


    Index GeneticAlgorithm::get_genes_number() const
    {
        return population.dimension(1);
    }


    /// Returns the rate used in the mutation.

    const type& GeneticAlgorithm::get_mutation_rate() const
    {
        return mutation_rate;
    }


    /// Returns the size of the elite in the selection.

    const Index& GeneticAlgorithm::get_elitism_size() const
    {
        return elitism_size;
    }


    /// Returns the method used for initalizating the population

    const GeneticAlgorithm::InitializationMethod& GeneticAlgorithm::get_initialization_method() const
    {
        return initialization_method;
    }


    /// Sets the members of the genetic algorithm object to their default values.

    void GeneticAlgorithm::set_default()
    {
        //First we set genes_number equals number of variables

        Index genes_number;

        if (training_strategy_pointer == nullptr || !training_strategy_pointer->has_neural_network())
        {
            genes_number = 0;
        }
        else
        {
            //"Variables" includes dummy variables, columns treat categorical variables as one (important)

            genes_number = training_strategy_pointer->get_data_set_pointer()->get_input_variables_number();
        }


        Index individuals_number = 10;

        maximum_epochs_number = 100;

        mutation_rate = static_cast<type>(0.01);

        // Population stuff

        population.resize(individuals_number, genes_number);

        parameters.resize(individuals_number);
        for (Index i = 0; i < individuals_number; i++) parameters(i).resize(genes_number);

        training_errors.resize(individuals_number);

        selection_errors.resize(individuals_number);

        fitness.resize(individuals_number);

        fitness.setConstant(type(-1.0));

        selection.resize(individuals_number);

        // Training operators

        elitism_size = 2;

    }


    /// Sets a new popualtion.
    /// @param new_population New population matrix.

    void GeneticAlgorithm::set_population(const Tensor<bool, 2>& new_population)
    {
#ifdef OPENNN_DEBUG

        const Index individuals_number = get_individuals_number();
        const Index new_individuals_number = new_population.dimension(1);

        // Optimization algorithm

        ostringstream buffer;

        if (!training_strategy_pointer)
        {
            buffer << "OpenNN Exception: InputsSelection class.\n"
                << "void check() const method.\n"
                << "Pointer to training strategy is nullptr.\n";

            throw invalid_argument(buffer.str());
        }

        // Loss index

        const LossIndex* loss_index_pointer = training_strategy_pointer->get_loss_index_pointer();

        if (!loss_index_pointer)
        {
            buffer << "OpenNN Exception: InputsSelection class.\n"
                << "void check() const method.\n"
                << "Pointer to loss index is nullptr.\n";

            throw invalid_argument(buffer.str());
        }

        // Neural network

        const NeuralNetwork* neural_network_pointer = loss_index_pointer->get_neural_network_pointer();

        if (!neural_network_pointer)
        {
            buffer << "OpenNN Exception: InputsSelection class.\n"
                << "void check() const method.\n"
                << "Pointer to neural network is nullptr.\n";

            throw invalid_argument(buffer.str());
        }

        if (new_individuals_number != individuals_number)
        {
            ostringstream buffer;

            buffer << "OpenNN Exception: GeneticAlgorithm class.\n"
                << "void set_population(const Tensor<type, 2>&) method.\n"
                << "Population rows(" << new_individuals_number << ") must be equal to population size(" << individuals_number << ").\n";

            throw invalid_argument(buffer.str());
        }

#endif

        population = new_population;
    }

    void GeneticAlgorithm::set_maximum_epochs_number(const Index& new_maximum_epochs_number)
    {
        maximum_epochs_number = new_maximum_epochs_number;


    }


    void GeneticAlgorithm::set_training_errors(const Tensor<type, 1>& new_training_errors)
    {
        training_errors = new_training_errors;
    }



    void GeneticAlgorithm::set_selection_errors(const Tensor<type, 1>& new_selection_errors)
    {
        selection_errors = new_selection_errors;
    }


    /// Sets a new fitness for the population.
    /// @param new_fitness New fitness values.

    void GeneticAlgorithm::set_fitness(const Tensor<type, 1>& new_fitness)
    {
#ifdef OPENNN_DEBUG

        const Index individuals_number = get_individuals_number();

        if (new_fitness.size() != individuals_number)
        {
            ostringstream buffer;

            buffer << "OpenNN Exception: GeneticAlgorithm class.\n"
                << "void set_fitness(const Tensor<type, 1>&) method.\n"
                << "Fitness size (" << new_fitness.size()
                << ") must be equal to population size (" << individuals_number << ").\n";

            throw invalid_argument(buffer.str());
        }

        for (Index i = 0; i < individuals_number; i++)
        {
            if (new_fitness[i] < 0)
            {
                ostringstream buffer;

                buffer << "OpenNN Exception: GeneticAlgorithm class.\n"
                    << "void set_fitness(const Tensor<type, 2>&) method.\n"
                    << "Fitness must be greater than 0.\n";

                throw invalid_argument(buffer.str());
            }
        }

#endif

        fitness = new_fitness;
    }

    /// Sets a new population size. It must be greater than 4.
    /// @param new_population_size Size of the population

    void GeneticAlgorithm::set_individuals_number(const Index& new_individuals_number)
    {
#ifdef OPENNN_DEBUG

        if (new_individuals_number < 4)
        {
            ostringstream buffer;

            buffer << "OpenNN Exception: GeneticAlgorithm class.\n"
                << "void set_individuals_number(const Index&) method.\n"
                << "Population size (" << new_individuals_number << ") must be greater than 4.\n";

            throw invalid_argument(buffer.str());
        }

#endif

        Index new_genes_number = training_strategy_pointer->get_data_set_pointer()->get_input_variables_number();

        population.resize(new_individuals_number, new_genes_number);

        parameters.resize(new_individuals_number);

        training_errors.resize(new_individuals_number);
        selection_errors.resize(new_individuals_number);

        fitness.resize(new_individuals_number);
        fitness.setConstant(type(-1.0));

        selection.resize(new_individuals_number);

        if (elitism_size > new_individuals_number) elitism_size = new_individuals_number;
    }


    /// Sets a new initalization method.
    /// @param new_initializatio_method New initalization method (Random or WeigthedCorrelations).

    void GeneticAlgorithm::set_initialization_method(const GeneticAlgorithm::InitializationMethod& new_initialization_method)
    {
        initialization_method = new_initialization_method;
    }


    /// Sets a new rate used in the mutation.
    /// It is a number between 0 and 1.
    /// @param new_mutation_rate Rate used for the mutation.

    void GeneticAlgorithm::set_mutation_rate(const type& new_mutation_rate)
    {
#ifdef OPENNN_DEBUG

        if (new_mutation_rate < 0 || new_mutation_rate > 1)
        {
            ostringstream buffer;

            buffer << "OpenNN Exception: GeneticAlgorithm class.\n"
                << "void set_mutation_rate(const type&) method.\n"
                << "Mutation rate must be between 0 and 1.\n";

            throw invalid_argument(buffer.str());
        }

#endif

        mutation_rate = new_mutation_rate;
    }


    /// Sets the number of individuals with the greatest fitness selected.
    /// @param new_elitism_size Size of the elitism.

    void GeneticAlgorithm::set_elitism_size(const Index& new_elitism_size)
    {
#ifdef OPENNN_DEBUG

        const Index individuals_number = get_individuals_number();

        if (new_elitism_size > individuals_number)
        {
            ostringstream buffer;

            buffer << "OpenNN Exception: GeneticAlgorithm class.\n"
                << "void set_elitism_size(const Index&) method.\n"
                << "Elitism size(" << new_elitism_size
                << ") must be lower than the population size(" << individuals_number << ").\n";

            throw invalid_argument(buffer.str());
        }

#endif

        elitism_size = new_elitism_size;
    }


    /// Initialize the population depending on the intialization method.

    void GeneticAlgorithm::initialize_population()
    {
#ifdef OPENNN_DEBUG

        const Index individuals_number = get_individuals_number();

        if (individuals_number == 0)
        {
            ostringstream buffer;

            buffer << "OpenNN Exception: GeneticAlgorithm class.\n"
                << "void initialize_population() method.\n"
                << "Population size must be greater than 0.\n";

            throw invalid_argument(buffer.str());
        }

        const Index genes_number = get_genes_number();

        if (individuals_number > pow(2, genes_number))
        {
            ostringstream buffer;

            buffer << "OpenNN Exception: GeneticAlgorithm class.\n"
                << "void initialize_population() method.\n"
                << "Individuals number must be less than 2 to the power of genes number.\n";

            throw invalid_argument(buffer.str());
        }

#endif


        if (initialization_method == GeneticAlgorithm::InitializationMethod::Random)
        {
            initialize_population_random();
        }
        else
        {
            initialize_population_correlations();
        }



    }


    ///Generation of a random population
    void GeneticAlgorithm::initialize_population_random()
    {
        ///Parameters obtention
        DataSet* data_set_pointer=training_strategy_pointer->get_data_set_pointer();

        const Index columns_number=data_set_pointer->get_input_columns_number();

        const Index individuals_number = get_individuals_number();

        const Index genes_number = get_genes_number();

        //First we set population as false
        population.setConstant(false);

        //Generate a new random seed that depends on the execution time, making each execution different
        srand(time(nullptr));

        //This variable controls if the generated individual is repeated or empty

        bool is_repeated_or_empty=false;

        Tensor<bool, 1> individual_variables(genes_number);

        Tensor<bool,1> individual_columns(columns_number);

        Index active_columns_count=0;
        Index active_columns;

        //
        if(display)
        {
            cout<<"Creating initial random population"<<endl;
        }

        for(Index i=0; i<individuals_number;i++)
        {

            do{

                individual_columns.setConstant(false);

                individual_variables.setConstant(false);

                //This variable represents how many activated columns will have this individual
                active_columns=1+rand()%columns_number;

                active_columns_count=0;

                type random_column;
                while(active_columns_count<active_columns)
                {
                    random_column=rand()%columns_number;
                    if(!individual_columns(random_column))
                    {
                        individual_columns(random_column)=true;
                        active_columns_count++;
                    }

                }
                //Check for no inputs
                if(is_false(individual_columns))
                {
                     is_repeated_or_empty=true;
                }
                //Check for repetition
                 individual_variables=get_individual_as_variables_from_columns(individual_columns);
                for(Index j=0;j<i;j++)
                {
                    Tensor<bool,1> row=population.chip(j,0);
                    if(are_equal(individual_variables,row))
                    {
                        is_repeated_or_empty=true;
                    }
                }

            }while(is_repeated_or_empty);
            //cout<< "Individual "<< i+1<<" creado"<<endl;

            //Add individual to population
            for(Index j=0;j<genes_number;j++)
            {
                population(i,j)=individual_variables(j);
            }
        }

        if(display)
        {
           cout<<"Initial random population created"<<endl;

        }
      }

    void GeneticAlgorithm::initialize_population_correlations()
    {
        //Needed parameters obtentions

        srand(static_cast<unsigned>(time(nullptr)));

        Index individuals_number = get_individuals_number();

        const Index genes_number = get_genes_number();///= number of inputs including dummy variables

        DataSet* data_set_pointer=training_strategy_pointer->get_data_set_pointer();

        const Index columns_number=data_set_pointer->get_input_columns_number(); ///In datasets without categorical variables==genes_number

        cout<<"Calculating correlations matrix"<<endl;

        Tensor<Correlation, 2> correlations_matrix = data_set_pointer->calculate_input_target_columns_correlations();

        cout<<"Correlation matrix calculated"<<endl;


        Tensor<type, 1> correlations = get_correlation_values(correlations_matrix).chip(0, 1);

        Tensor<Index, 1> rank(genes_number);



        rank= calculate_rank_greater(correlations);


        Tensor<type, 1 > fitness_correlations(rank.size());



         for(Index i=0;i<columns_number;i++)
        {
            fitness_correlations(rank(i))=type(i+1);
        }///End of calculation of the new fitness correlations vector


        ///Cumulative probability tensor calculation

        type sum = (type(columns_number)*type(columns_number+1))/2;

        ///This vector stores in probabilities_vector(i)="Probability of input number i being choose"
        ///

        Tensor <type,1> probabilities_vector(columns_number);


        for (Index i = 0; i <columns_number ; i++)
        {
            probabilities_vector[i] = type(fitness_correlations.size() - fitness_correlations(i)-1) / sum;

        }

        ///This tensor means cumulative_probabilities(i)="sum of the first i elements of probabilities_vector"
        Tensor <type, 1> cumulative_probabilities = probabilities_vector.cumsum(0);

        Tensor<Index, 1> columns_variables_indices(columns_number);

        Index index_count=0;
        for(Index i=0;i<columns_number;i++)
        {
             if(data_set_pointer->get_column_type(i)==DataSet::ColumnType::Categorical)
             {
                columns_variables_indices(i)=index_count;

                index_count+=data_set_pointer->get_columns()(i).get_categories_number();
             }else
             {
                columns_variables_indices(i)=index_count;
                index_count++;
             }
         }



        Tensor <bool, 1> individual(genes_number);
        Index activated_columns;
        Index columns_count;
        type pointer;

        ///Population Generation
        for(Index i=0; i<individuals_number;i++)
        {
            individual.setConstant(false);
            bool is_repeated=false;
            do
            {
                activated_columns=1+rand()%columns_number;
                columns_count=0;
                do
                {
                    ///Random pointer generation
                    pointer=type(rand())/type(RAND_MAX);
                    if(pointer<cumulative_probabilities(0)&&!individual(0))
                    {
                        ///Categorical comprobation
                        if(data_set_pointer->get_column_type(0)==DataSet::ColumnType::Categorical)
                        {
                            for(Index j=0;j<data_set_pointer->get_columns()(0).get_categories_number();j++)
                            {
                                individual(j)=true;
                            }
                            columns_count++;
                        }else
                        {
                            individual(0)=true;
                           columns_count++;
                        }
                    }else
                    {
                        for(Index j=0;j<columns_number;j++)
                        {
                            if(pointer>cumulative_probabilities(j) && pointer<=cumulative_probabilities(j+1)&& !individual(columns_variables_indices(j)))
                            {
                                if(data_set_pointer->get_column_type(j)==DataSet::ColumnType::Categorical)
                                {
                                    for(Index k=0;k<data_set_pointer->get_columns()(j).get_categories_number();k++)
                                    {
                                        individual(columns_variables_indices(j)+k)=true;
                                        columns_count++;
                                    }
                                }else
                                {
                                    individual(j)=true;
                                    columns_count++;
                                }
                            }
                        }
                    }



                }while(columns_count<activated_columns);



            ///Check for repetitions
            for (Index j = 0; j < i; j++)
                {
                    Tensor<bool, 1> row = population.chip(j, 0);

                    if (are_equal(individual, row))
                    {
                            is_repeated = true;
                            //break;
                    }
                }
            ///Check for no inputs
            if(is_false(individual))
            {
                is_repeated=true;
            }

            }while(is_repeated);


            ///Individual addition to population
            for(Index j=0;j<genes_number;j++)
            {
                population(i,j)=individual(j);
            }

        }




    }

    /// Evaluate the population loss.
    /// Training all the neural networks in the population and calculate their fitness.

    void GeneticAlgorithm::evaluate_population()
    {
#ifdef OPENNN_DEBUG

        check();

        if (population.size() == 0)
        {
            ostringstream buffer;

            buffer << "OpenNN Exception: GeneticAlgorithm class.\n"
                << "void evaluate_population() method.\n"
                << "Population size must be greater than 0.\n";

            throw invalid_argument(buffer.str());
        }

#endif

        // Training strategy

        TrainingResults training_results;

        // Loss index

        const LossIndex* loss_index_pointer = training_strategy_pointer->get_loss_index_pointer();

        // Data set

        DataSet* data_set_pointer = loss_index_pointer->get_data_set_pointer();

        original_input_columns_indices = data_set_pointer->get_input_columns_indices();
        original_target_columns_indices = data_set_pointer->get_target_columns_indices();

        Tensor<string, 1> inputs_variables_names;

        // Neural network

        NeuralNetwork* neural_network_pointer = loss_index_pointer->get_neural_network_pointer();

        // Optimization algorithm

        Tensor<bool, 1> individual;

        // Model selection

        const Index individuals_number = get_individuals_number();
        const Index genes_number = get_genes_number();
        Tensor<bool,1> individual_columns;
        Tensor<Index,1> individual_columns_indexes;
        //Tensor<string,1>inputs_names;
        Tensor<Index,1> inputs_activated(individuals_number);
        set_display(true);

        for (Index i = 0; i < individuals_number; i++)
        {

            individual = population.chip(i, 0);


            //if (display) cout << "Individual " << i + 1 << endl;

            individual_columns_indexes=get_individual_as_columns_indexes_from_variables(individual);

            inputs_activated(i)=individual_columns_indexes.size();

            data_set_pointer->set_input_target_columns(individual_columns_indexes, original_target_columns_indices);

            Tensor<string,1> inputs_names = data_set_pointer->get_input_variables_names();

            neural_network_pointer->set_inputs_number(data_set_pointer->get_input_variables_number());

            neural_network_pointer->set_inputs_names(inputs_names);

            //cout<<neural_network_pointer->get_inputs_names()<<endl;

            //system("pause");

            neural_network_pointer->set_parameters_random();

            training_strategy_pointer->set_display(false);
            training_results = training_strategy_pointer->perform_training();


            parameters(i) = neural_network_pointer->get_parameters();

            training_errors(i) = type(training_results.get_training_error());

            selection_errors(i) = type(training_results.get_selection_error());


           /*   if (display)
            {
                cout << "Inputs: " << endl;

                individual_columns=get_individual_as_columns_from_variables(individual);

                for(Index i=0;i<individual_columns.size();i++)
                {
                    if(individual_columns(i))
                    {

                        cout<<data_set_pointer->get_columns()(i).name<<endl;
                    }
                }


                //cout<< inputs_names<<endl;

                //for (Index i = 0; i < inputs_names.size(); i++) cout << "   " << inputs_names(i) << endl;

                cout << "Training error: " << training_results.get_training_error() << endl;
                cout << "Selection error: " << training_results.get_selection_error() << endl;

            }*/
            data_set_pointer->set_input_target_columns(original_input_columns_indices,original_target_columns_indices);


        }

        //Mean generational selection and training error calculation (primitive way)

        type sum1 = 0;

        type sum2 = 0;

        type sum3=0; //Alberga la suma de las inputs activadas por generación

        for (Index i = 0; i < individuals_number; i++)
            {
                sum1 += training_errors(i);
                sum2 += selection_errors(i);
                sum3 += inputs_activated(i);

            }
        mean_generational_training_error = (type(sum1) / type(training_errors.size()));
        mean_generational_selection_error = (type(sum2) / type(selection_errors.size()));
        mean_generational_inputs_activated=(type(sum3)/type(individuals_number));
}





    /// Calculate the fitness with the errors depending on the fitness assignment method.

    void GeneticAlgorithm::perform_fitness_assignment()
    {
        const Index individuals_number = get_individuals_number();

        const Tensor<Index, 1> rank = calculate_rank_less(selection_errors);

        for (Index i = 0; i < individuals_number; i++)
        {
            fitness(rank(i)) = type(i + 1);
        }


    }


    /// Selects for crossover some individuals from the population.

    void GeneticAlgorithm::perform_selection()
    {
#ifdef OPENNN_DEBUG

        if (population.size() == 0)
        {
            ostringstream buffer;

            buffer << "OpenNN Exception: GeneticAlgorithm class.\n"
                << "void perform_selection() method.\n"
                << "Population size must be greater than 0.\n";

            throw invalid_argument(buffer.str());
        }

        if (fitness.dimension(0) == 0)
        {
            ostringstream buffer;

            buffer << "OpenNN Exception: GeneticAlgorithm class.\n"
                << "void perform_selection() method.\n"
                << "No fitness found.\n";

            throw invalid_argument(buffer.str());
        }

#endif

        selection.resize(get_individuals_number());

        selection.setConstant(false);

        const Index individuals_number=get_individuals_number();

        Index selected_individuals_number = static_cast<Index>(type(individuals_number)/2);


        /*cout<<static_cast<Index>(individuals_number/2)<<endl;
        cout<<"Tamaño de elitismo: "<<elitism_size<<endl;
        cout<<"Individuos seleccionados: "<<selected_individuals_number<<endl;*/

        //Calculation of cumulative probabilities
        Index sum=0;
        for(Index i=0;i<individuals_number;i++)
        {
            sum+=(i+1);
        }

        Tensor<type,1> probabilities(individuals_number);

        for(Index i=0;i<individuals_number;i++)
        {
            probabilities(i)=(type(individuals_number)-type(fitness(i)-1))/sum;
        }

        Tensor<type,1> cumulative_probabilities=probabilities.cumsum(0);

        ///Elitism
        if (elitism_size != 0)
        {
            for (Index i = 0; i < individuals_number; i++)
            {
                if (fitness(i) - 1 >=0 && fitness(i) - 1 <= elitism_size)
                {
                    selection(i) = true;
                }
            }
        }

        srand(static_cast<unsigned>(time(nullptr)));

        Index selected_individuals_count=count(selection.data(), selection.data() + selection.size(), 1);

        type pointer;

        while(selected_individuals_count<selected_individuals_number)
        {

            pointer=type(rand())/type(RAND_MAX);
            if(pointer<cumulative_probabilities(0)&& !selection(0))
            {
                selection(0)=true;
                selected_individuals_count++;
            }
            for(Index i=0;i<individuals_number-1;i++)
            {
                if(pointer>=cumulative_probabilities(i) && pointer<cumulative_probabilities(i+1) && !selection(i) )
                {
                    selection(i)=true;
                    selected_individuals_count++;
                    break;
                }
            }

        }


#ifdef OPENNN_DEBUG

        Index selection_assert = 0;
        for (Index i = 0; i < individuals_number; i++) if (selection(i)) selection_assert++;

        if (selection_assert != individuals_number / 2)
        {
            ostringstream buffer;

            buffer << "OpenNN Exception: GeneticAlgorithm class.\n"
                << "void perform_selection() method.\n"
                << "Number of selected individuals (" << selection_assert << ") must be " << individuals_number / 2 << " .\n";

            throw invalid_argument(buffer.str());
        }

#endif

    }

    ///Transform selection vector to indices

    Tensor <Index,1> GeneticAlgorithm::get_selected_individuals_to_indexes()
    {
        Tensor<Index,1> selection_indexes(count(selection.data(), selection.data() + selection.size(), 1));
        Index activated_index_count=0;
        for(Index i=0;i<selection.size();i++)
        {
            if(selection(i))
            {
                selection_indexes(activated_index_count)=i;
                activated_index_count++;
            }
        }
        return selection_indexes;

    }

    /// Perform the crossover depending on the crossover method.

    void GeneticAlgorithm::perform_crossover()
    {
        const Index individuals_number = get_individuals_number();
        const Index genes_number = get_genes_number();
        const Index selected_individuals_number = std::count(selection.data(), selection.data() + selection.size(), 1);
        #ifdef OPENNN_DEBUG
                Index count_selected_individuals = 0;
                for (Index i = 0; i < individuals_number; i++) if (selection(i)) count_selected_individuals++;
                if (selected_individuals_number != count_selected_individuals)
                {
                    ostringstream buffer;
                    buffer << "OpenNN Exception: GeneticAlgorithm class.\n"
                        << "void perform_crossover() method.\n"
                        << "Selected individuals number is wrong.\n";
                    throw invalid_argument(buffer.str());
                }
        #endif
        ///Couples generation
        /// First obtain selected individuals indexes
        Tensor<Index,1> parent_1_indexes=get_selected_individuals_to_indexes();
        Tensor<Index, 1> parent_2_indexes=parent_1_indexes;
        std::random_device rd;
        std::mt19937 g(rd());


        Tensor<Index,2> couples(selected_individuals_number,2);
        bool is_parent_repeated=false;
        bool is_couple_repeated=false;

        for(Index i=0;i<selected_individuals_number;i++)
        {

            do{
                is_couple_repeated=false;
                is_parent_repeated=false;

                std::shuffle(parent_2_indexes.data(),parent_2_indexes.data()+parent_2_indexes.size(), g);

                if(parent_1_indexes(i)==parent_2_indexes(i))
                {
                    is_parent_repeated=true;


                }

                //Check for couples repetition
                for(Index j=0;j<i;j++)
                {
                    if((couples(j,0)==parent_1_indexes(i) && couples(j,1)==parent_2_indexes(i))||(couples(j,0)==parent_2_indexes(i) && couples(j,1)==parent_1_indexes(i)))
                    {
                       is_couple_repeated=true;

                    }
                }

            }while(is_parent_repeated||is_couple_repeated);
            //couple to collection addition
            couples(i,0)=parent_1_indexes(i);
            couples(i,1)=parent_2_indexes(i);
        }


        Tensor<bool, 1> parent_1_variables;
        Tensor<bool, 1> parent_2_variables;
        Tensor<bool,1> descendent_variables;
        Tensor<bool, 1> parent_1_columns;
        Tensor<bool, 1> parent_2_columns;
        Tensor<bool, 1> descendent_columns;
        Tensor<bool, 2> new_population(individuals_number, genes_number);
        Index offspring_count=0;
        bool is_empty=false;

        //We keep parents in the new generation
        for (Index i = 0; i < individuals_number; i++)
        {
           if (selection(i) && offspring_count < selected_individuals_number)
            {
              for (Index j = 0; j < genes_number; j++)
              {
                new_population(offspring_count, j) = population(i, j);
              }
              offspring_count++;
            }
         }
        for(Index i=0;i< selected_individuals_number;i++)
        {
            parent_1_variables=population.chip(couples(i,0),0);
            parent_2_variables=population.chip(couples(i,1),0);
            ///Transform each parent to columns
            parent_1_columns=get_individual_as_columns_from_variables(parent_1_variables);
            parent_2_columns=get_individual_as_columns_from_variables(parent_2_variables);
            //Perform crossover
            do{
                is_empty=false;
                descendent_columns=parent_1_columns;
                for(Index j=0;j<descendent_columns.size();j++)
                {
                    if(parent_1_columns(j)!=parent_2_columns(j))
                    {
                        rand()%2==0 ? descendent_columns(j)=false : descendent_columns(j)=true;
                    }
                }
                //check for repetitions
            for(Index j=0;j<i;j++)
                {
                    Tensor<bool,1> row=new_population.chip(j,0);
                    descendent_variables=get_individual_as_variables_from_columns(descendent_columns);
                    if(are_equal(row,descendent_variables))
                    {
                        is_empty=true;
                    }
                }
                ///Prevent no inputs
                if(is_false(descendent_columns))
                {
                    is_empty=true;
                }

            }while(is_empty);
            ///Population addition
            Tensor<bool,1> descendent=get_individual_as_variables_from_columns(descendent_columns);
            for(Index j=0;j<genes_number;j++)
            {
                new_population(i+selected_individuals_number,j)=descendent(j);

            }


            //cout<<descendent_columns;


        }
        population=new_population;


 }





    /// Perform the mutation of the individuals generated in the crossover.

    void GeneticAlgorithm::perform_mutation()
    {
        const Index individuals_number=get_individuals_number();

        const Index genes_number=get_genes_number();

        Tensor<bool,1> individual_variables;

        Tensor<bool,1> individual_columns;


        for(Index i=0; i<individuals_number;i++)
        {
            individual_variables=population.chip(i,0);

            individual_columns=get_individual_as_columns_from_variables(individual_variables);

            bool is_empty;
           do{
                is_empty=false;
            for (Index j = 0; j <individual_columns.size();j++)
            {
                if ((static_cast<type>(rand() / (RAND_MAX + 1.0)) <= mutation_rate))
                {
                    individual_columns(j)= !individual_columns(j);
                }
            }
            ///Prevent no inputs
            if(is_false(individual_columns))
            {
                is_empty=true;
            }
            }while(is_empty);

            individual_variables=get_individual_as_variables_from_columns(individual_columns);

            for(Index j=0;j<genes_number;j++)
            {
                population(i,j)=individual_variables(j);
            }


        }

    }

    void GeneticAlgorithm::print_population()
    {
        Tensor <bool, 2 > population = get_population();
        Index individuals_number = get_individuals_number();
        Index genes_number = get_genes_number();

        for (Index i = 0; i < individuals_number; i++)
        {
            cout << "\n Individuo " << i + 1 << " \n";
            for (Index j = 0; j < genes_number; j++)
            {
                if (population(i, j))
                {
                    cout << "La input X_" << j + 1 << "esta seleccionada" << endl;
                }
                else
                {
                    cout << "La input X_" << j + 1 << " no esta seleccionada" << endl;
                }

            }
        }
    }



    /// Select the inputs with the best generalization properties using the genetic algorithm.

    InputsSelectionResults GeneticAlgorithm::perform_inputs_selection()
    {
#ifdef OPENNN_DEBUG

        check();

#endif



        if (display) cout << "Performing genetic inputs selection..." << endl << endl;


        initialize_population();


        // Selection algorithm

        InputsSelectionResults inputs_selection_results(maximum_epochs_number);

        // Training strategy

        training_strategy_pointer->set_display(false);

        // Loss index

        const LossIndex* loss_index_pointer = training_strategy_pointer->get_loss_index_pointer();

        // Data set

        DataSet* data_set_pointer = loss_index_pointer->get_data_set_pointer();

        // Neural network

        NeuralNetwork* neural_network_pointer = loss_index_pointer->get_neural_network_pointer();

        // Optimization algorithm

        Index optimal_individual_index;

        bool stop = false;

        time_t beginning_time;
        time_t current_time;
        type elapsed_time = type(0);

        original_input_columns_indices=data_set_pointer->get_input_variables_indices();
        original_target_columns_indices=data_set_pointer->get_target_variables_indices();
        Tensor<Index,1> optimal_inputs_columns_indexes;
        time(&beginning_time);
        inputs_selection_results.optimum_selection_error=numeric_limits<type>::max();
        training_strategy_pointer->get_optimization_algorithm_pointer()->set_display(false);
        /*if(display)
        {
            cout<< "Generation 0 "<<endl;
           cout<< "Mean inputs number  " << mean_generational_inputs_activated<<endl;
           system("pause");
        }*/

        for (Index epoch = 0; epoch < maximum_epochs_number; epoch++)
        {
            if (display) cout << "Generation: " << epoch + 1 << endl;
            inputs_selection_results.resize_history(inputs_selection_results.mean_training_error_history.size()+1);

            evaluate_population();



            ///Optimal individual in population
            optimal_individual_index = minimal_index(selection_errors);



            ///Store optimal individual in the history

            /*for(Index i=0;i<population.dimension(1);i++)
            {
            optimal_individuals_history(epoch,i)=population(optimal_individual_index,i);
            }
            system("pause");*/


            ///store optimal training and selection error in the history

            inputs_selection_results.training_error_history(epoch) = training_errors(optimal_individual_index);



            inputs_selection_results.selection_error_history(epoch) = selection_errors(optimal_individual_index);

            ///store mean errors histories

            inputs_selection_results.mean_selection_error_history(epoch) = mean_generational_selection_error;


            inputs_selection_results.mean_training_error_history(epoch)= mean_generational_training_error;

            if (selection_errors(optimal_individual_index) < inputs_selection_results.optimum_selection_error)
            {
                data_set_pointer->set_input_target_columns(original_input_columns_indices, original_target_columns_indices);

                // Neural network

                inputs_selection_results.optimal_inputs = population.chip(optimal_individual_index, 0);

                //inputs_selection_results.optimal_inputs = transform_individual_to_indexes(inputs_selection_results.optimal_inputs);

                optimal_inputs_columns_indexes=get_individual_as_columns_indexes_from_variables(inputs_selection_results.optimal_inputs);

                data_set_pointer->set_input_target_columns(optimal_inputs_columns_indexes,original_target_columns_indices);

                inputs_selection_results.optimal_input_columns_names = data_set_pointer->get_input_columns_names();

                inputs_selection_results.optimal_parameters = parameters(optimal_individual_index);

                // Loss index

                inputs_selection_results.optimum_training_error = training_errors(optimal_individual_index);

                inputs_selection_results.optimum_selection_error = selection_errors(optimal_individual_index);

            }else{
                data_set_pointer->set_input_target_columns(original_input_columns_indices,original_target_columns_indices);
            }

            // Needed for new input indexes setting (is like a reset)


            data_set_pointer->set_input_target_columns(original_input_columns_indices, original_target_columns_indices);

            time(&current_time);

            elapsed_time = static_cast<type>(difftime(current_time, beginning_time));

            if (display)
            {
                cout << endl;

                /*cout << "Epoch number: " << epoch << endl;
                cout << "Generation mean training error: " << training_errors.mean() << endl;
                cout << "Generation mean selection error: " << inputs_selection_results.mean_selection_error_history(epoch) << endl;*/
                cout<< "Mean inputs number  " << mean_generational_inputs_activated<<endl;


                /*cout << "Generation minimum training error: " << training_errors(optimal_individual_index) << endl;
                cout << "Generation minimum selection error: " << selection_errors(optimal_individual_index) << endl;

                cout << "Best ever training error: " << inputs_selection_results.optimum_training_error << endl;
                cout << "Best ever selection error: " << inputs_selection_results.optimum_selection_error << endl;

                cout << "Elapsed time: " << write_time(elapsed_time) << endl;*/

            }

            // Stopping criteria

            if (elapsed_time >= maximum_time)
            {
                stop = true;

                if (display) cout << "Epoch " << epoch << endl << "Maximum time reached: " << write_time(elapsed_time) << endl;

                inputs_selection_results.stopping_condition = InputsSelection::StoppingCondition::MaximumTime;
            }

            if (selection_errors(optimal_individual_index) <= selection_error_goal) // ???
            {
                stop = true;

                if (display) cout << "Epoch  " << epoch << endl << "Selection error reached: " << selection_errors(optimal_individual_index) << endl;

                inputs_selection_results.stopping_condition = InputsSelection::StoppingCondition::SelectionErrorGoal;
            }

            if (epoch >= maximum_epochs_number - 1)
            {
                stop = true;

                if (display) cout << "Epoch " << epoch << endl << "Maximum number of epochs reached: " << epoch << endl;

                inputs_selection_results.stopping_condition = InputsSelection::StoppingCondition::MaximumEpochs;
            }

            if (stop)
            {
                inputs_selection_results.elapsed_time = write_time(elapsed_time);

                inputs_selection_results.resize_history(epoch + 1);

                break;
            }

            perform_fitness_assignment();

            perform_selection();

            perform_crossover();

            if(mutation_rate!=0){
            perform_mutation();
            }

            //check_categorical_columns();

        }

        // Set data set stuff


        Tensor<Index,1>optimal_columns=get_individual_as_columns_indexes_from_variables(inputs_selection_results.optimal_inputs);

        data_set_pointer->set_input_target_columns(optimal_columns,original_target_columns_indices);

        const Tensor<Scaler, 1> input_variables_scalers = data_set_pointer->get_input_variables_scalers();

        const Tensor<Descriptives, 1> input_variables_descriptives = data_set_pointer->calculate_input_variables_descriptives();

        // Set neural network stuff

        neural_network_pointer->set_inputs_number(data_set_pointer->get_input_variables_number());

        neural_network_pointer->set_inputs_names(data_set_pointer->get_input_variables_names());

        if (neural_network_pointer->has_scaling_layer())
            neural_network_pointer->get_scaling_layer_pointer()->set(input_variables_descriptives, input_variables_scalers);

        neural_network_pointer->set_parameters(inputs_selection_results.optimal_parameters);

        if (display) inputs_selection_results.print();



        return inputs_selection_results;
    }



    void GeneticAlgorithm::check_categorical_columns()
    {
        TrainingStrategy* training_strategy_pointer = get_training_strategy_pointer();

        DataSet* data_set_pointer = training_strategy_pointer->get_data_set_pointer();

        const Index individuals_number = get_individuals_number();

        const Index variables_number = data_set_pointer->get_input_variables_number();

        Index column_index = 0;

        if (data_set_pointer->has_categorical_columns())
        {
            for (Index i = 0; i < variables_number; i++)
            {
                const DataSet::ColumnType column_type = data_set_pointer->get_column_type(column_index);

                if (column_type != DataSet::ColumnType::Categorical)
                {
                    column_index++;
                    continue;
                }

                const Index categories_number = data_set_pointer->get_columns()(column_index).get_categories_number();

                for (Index j = 0; j < individuals_number; j++)
                {
                    const Tensor<bool, 1> individual = population.chip(j, 0);

                    if (!(std::find(individual.data() + i, individual.data() + i + categories_number, 1) == (individual.data() + i + categories_number)))
                    {
                        const Index random_index = rand() % categories_number;

                        for (Index categories_index = 0; categories_index < categories_number; categories_index++)
                        {
                            population(j, i + categories_index) = false;
                        }

                        population(j, i + random_index) = true;
                    }
                }

                i += categories_number -1;
                column_index++;
            }
        }

    };

    Tensor<bool,1 > GeneticAlgorithm::get_individual_as_columns_from_variables(Tensor<bool,1>& individual)
    {
        DataSet* data_set_pointer=training_strategy_pointer->get_data_set_pointer();
        const Index columns_number=data_set_pointer->get_input_columns_number();
        Tensor<bool,1> columns_indexes(columns_number);
        columns_indexes.setConstant(false);
        Index genes_count=0;
        Index categories_number;
        for(Index i=0;i<columns_number;i++)
        {
            if(data_set_pointer->get_column_type(i)==DataSet::ColumnType::Categorical)
            {
                categories_number=data_set_pointer->get_columns()(i).get_categories_number();
                for(Index j=0;j<categories_number;j++)
                {
                    if(individual(genes_count+j))
                    {
                        columns_indexes(i)=true;
                    }
                }
                genes_count+=categories_number;

            }else
            {
                columns_indexes(i)=individual(genes_count);

                genes_count++;
            }
        }

        return columns_indexes;

    }
    Tensor<Index,1> GeneticAlgorithm::get_individual_as_columns_indexes_from_variables(Tensor<bool,1>& individual)
    {
        Tensor<bool, 1> individual_columns=get_individual_as_columns_from_variables(individual);
        Index active_columns=0;
        for(Index i=0; i<individual_columns.size();i++)
        {
            if(individual_columns(i)==true)
            {
                active_columns++;
            }
        }
        Tensor<Index,1> individual_columns_indexes(active_columns);
        Index active_column_index=0;

        for(Index i=0; i<individual_columns.size();i++)
        {
            if(individual_columns(i)==true)
            {
                individual_columns_indexes(active_column_index)=i;
                active_column_index++;
            }
        }
        //cout<<individual_columns_indexes<<endl;
        return individual_columns_indexes;
    }

    Tensor<bool, 1> GeneticAlgorithm::get_individual_as_variables_from_columns(Tensor<bool, 1>& individual_columns)
    {
        DataSet* data_set_pointer=training_strategy_pointer->get_data_set_pointer();

        const Index columns_number=data_set_pointer->get_input_columns_number();

        const Index genes_number=get_genes_number();

        Tensor<bool,1> individual_columns_to_variables(genes_number);

        individual_columns_to_variables.setConstant(false);

        Index input_index=0;

        for(Index i=0;i<columns_number;i++)
        {
            if(data_set_pointer->get_column_type(i)==DataSet::ColumnType::Categorical)
            {
                if(individual_columns(i)){
                for(Index j=0; j<data_set_pointer->get_columns()(i).get_categories_number();j++)
                {
                    individual_columns_to_variables(input_index+j)=true;

                }
                }
                input_index+=data_set_pointer->get_columns()(i).get_categories_number();

            }else if(individual_columns(i))
            {
                individual_columns_to_variables(input_index)=true;
                input_index++;
            }else
            {
                input_index++;
            }

        }

        return individual_columns_to_variables;
    }

    Tensor<bool, 1> GeneticAlgorithm::transform_individual_to_indexes(Tensor<bool, 1>& individual)
    {
        DataSet* data_set_pointer = training_strategy_pointer->get_data_set_pointer();

        const Index columns_number = data_set_pointer->get_input_columns_number();

        Tensor<bool, 1> new_indexes(individual);

        Index variable_index = 0;

        if (data_set_pointer->has_categorical_columns())
        {
            for (Index i = 0; i < columns_number; i++)
            {
                if (data_set_pointer->get_column_type(i) == DataSet::ColumnType::Categorical)
                {
                    const Index categories_number = data_set_pointer->get_columns()(i).get_categories_number();

                    if (!(find(individual.data() + variable_index, individual.data() + variable_index + categories_number, 1) == individual.data() + variable_index + categories_number))
                    {
                        new_indexes(i) = true;
                    }
                    else
                    {
                        new_indexes(i) = false;
                    }
                    variable_index += categories_number;
                }
                else
                {
                    new_indexes(i) = individual(variable_index);
                    variable_index++;
                }
            }
        }
        return new_indexes;

    };


    /// This method writes a matrix of strings the most representative atributes.
    /// @todo to many rows in string matrix.

    Tensor<string, 2> GeneticAlgorithm::to_string_matrix() const
    {
        const Index individuals_number = get_individuals_number();

        ostringstream buffer;

        Tensor<string, 1> labels(6);
        Tensor<string, 1> values(6);

        Tensor<string, 2> string_matrix(labels.size(), 2);

        // Population size

        labels(0) = "Population size";

        buffer.str("");
        buffer << individuals_number;
        values(0) = buffer.str();

        // Elitism size

        labels(1) = "Elitism size";

        buffer.str("");
        buffer << elitism_size;
        values(1) = buffer.str();

        // Mutation rate

        labels(2) = "Mutation rate";

        buffer.str("");
        buffer << mutation_rate;
        values(2) = buffer.str();

        // Selection loss goal

        labels(3) = "Selection loss goal";

        buffer.str("");
        buffer << selection_error_goal;
        values(3) = buffer.str();

        // Maximum Generations number

        labels(4) = "Maximum Generations number";

        buffer.str("");
        buffer << maximum_epochs_number;
        values(4) = buffer.str();

        // Maximum time

        labels(5) = "Maximum time";

        buffer.str("");
        buffer << maximum_time;
        values(5) = buffer.str();

        string_matrix.chip(0, 1) = labels;
        string_matrix.chip(1, 1) = values;

        return string_matrix;

    }


    /// Serializes the genetic algorithm object into an XML document of the TinyXML library without keeping the DOM
    /// tree in memory.
    /// See the OpenNN manual for more information about the format of this document.

    void GeneticAlgorithm::write_XML(tinyxml2::XMLPrinter& file_stream) const
    {
        const Index individuals_number = get_individuals_number();

        ostringstream buffer;

        file_stream.OpenElement("GeneticAlgorithm");

        // Population size

        file_stream.OpenElement("PopulationSize");

        buffer.str("");
        buffer << individuals_number;

        file_stream.PushText(buffer.str().c_str());

        file_stream.CloseElement();

        // Elitism size

        file_stream.OpenElement("ElitismSize");

        buffer.str("");
        buffer << elitism_size;

        file_stream.PushText(buffer.str().c_str());

        file_stream.CloseElement();

        // Mutation rate

        file_stream.OpenElement("MutationRate");

        buffer.str("");
        buffer << mutation_rate;

        file_stream.PushText(buffer.str().c_str());

        file_stream.CloseElement();

        // selection error goal

        file_stream.OpenElement("SelectionErrorGoal");

        buffer.str("");
        buffer << selection_error_goal;

        file_stream.PushText(buffer.str().c_str());

        file_stream.CloseElement();

        // Maximum iterations

        file_stream.OpenElement("MaximumGenerationsNumber");

        buffer.str("");
        buffer << maximum_epochs_number;

        file_stream.PushText(buffer.str().c_str());

        file_stream.CloseElement();

        // Maximum time

        file_stream.OpenElement("MaximumTime");

        buffer.str("");
        buffer << maximum_time;

        file_stream.PushText(buffer.str().c_str());

        file_stream.CloseElement();

        file_stream.CloseElement();
    }


    /// Deserializes a TinyXML document into this genetic algorithm object.
    /// @param document TinyXML document containing the member data.

    void GeneticAlgorithm::from_XML(const tinyxml2::XMLDocument& document)
    {
        const tinyxml2::XMLElement* root_element = document.FirstChildElement("GeneticAlgorithm");

        if (!root_element)
        {
            ostringstream buffer;

            buffer << "OpenNN Exception: GeneticAlgorithm class.\n"
                << "void from_XML(const tinyxml2::XMLDocument&) method.\n"
                << "GeneticAlgorithm element is nullptr.\n";

            throw invalid_argument(buffer.str());
        }

        // Population size
        {
            const tinyxml2::XMLElement* element = root_element->FirstChildElement("PopulationSize");

            if (element)
            {
                const Index new_population_size = static_cast<Index>(atoi(element->GetText()));

                try
                {
                    set_individuals_number(new_population_size);
                }
                catch (const invalid_argument& e)
                {
                    cerr << e.what() << endl;
                }
            }
        }

        // Mutation rate
        {
            const tinyxml2::XMLElement* element = root_element->FirstChildElement("MutationRate");

            if (element)
            {
                const type new_mutation_rate = static_cast<type>(atof(element->GetText()));

                try
                {
                    set_mutation_rate(new_mutation_rate);
                }
                catch (const invalid_argument& e)
                {
                    cerr << e.what() << endl;
                }
            }
        }

        // Elitism size
        {
            const tinyxml2::XMLElement* element = root_element->FirstChildElement("ElitismSize");

            if (element)
            {
                const Index new_elitism_size = static_cast<Index>(atoi(element->GetText()));

                try
                {
                    set_elitism_size(new_elitism_size);
                }
                catch (const invalid_argument& e)
                {
                    cerr << e.what() << endl;
                }
            }
        }

        // Display
        {
            const tinyxml2::XMLElement* element = root_element->FirstChildElement("Display");

            if (element)
            {
                const string new_display = element->GetText();

                try
                {
                    set_display(new_display != "0");
                }
                catch (const invalid_argument& e)
                {
                    cerr << e.what() << endl;
                }
            }
        }

        // selection error goal
        {
            const tinyxml2::XMLElement* element = root_element->FirstChildElement("SelectionErrorGoal");

            if (element)
            {
                const type new_selection_error_goal = static_cast<type>(atof(element->GetText()));

                try
                {
                    set_selection_error_goal(new_selection_error_goal);
                }
                catch (const invalid_argument& e)
                {
                    cerr << e.what() << endl;
                }
            }
        }

        // Maximum iterations number
        {
            const tinyxml2::XMLElement* element = root_element->FirstChildElement("MaximumGenerationsNumber");

            if (element)
            {
                const Index new_maximum_epochs_number = static_cast<Index>(atoi(element->GetText()));

                try
                {
                    set_maximum_epochs_number(new_maximum_epochs_number);
                }
                catch (const invalid_argument& e)
                {
                    cerr << e.what() << endl;
                }
            }
        }

        // Maximum correlation
        {
            const tinyxml2::XMLElement* element = root_element->FirstChildElement("MaximumCorrelation");

            if (element)
            {
                const type new_maximum_correlation = static_cast<type>(atof(element->GetText()));

                try
                {
                    set_maximum_correlation(new_maximum_correlation);
                }
                catch (const invalid_argument& e)
                {
                    cerr << e.what() << endl;
                }
            }
        }

        // Minimum correlation
        {
            const tinyxml2::XMLElement* element = root_element->FirstChildElement("MinimumCorrelation");

            if (element)
            {
                const type new_minimum_correlation = static_cast<type>(atof(element->GetText()));

                try
                {
                    set_minimum_correlation(new_minimum_correlation);
                }
                catch (const invalid_argument& e)
                {
                    cerr << e.what() << endl;
                }
            }
        }

        // Maximum time
        {
            const tinyxml2::XMLElement* element = root_element->FirstChildElement("MaximumTime");

            if (element)
            {
                const type new_maximum_time = type(atoi(element->GetText()));

                try
                {
                    set_maximum_time(new_maximum_time);
                }
                catch (const invalid_argument& e)
                {
                    cerr << e.what() << endl;
                }
            }
        }
    }


    void GeneticAlgorithm::print() const
    {
        cout << "Genetic algorithm" << endl;
        cout << "Individuals number: " << get_individuals_number() << endl;
        cout << "Genes number: " << get_genes_number() << endl;
    }


    /// Saves to an XML-type file the members of the genetic algorithm object.
    /// @param file_name Name of genetic algorithm XML-type file.

    void GeneticAlgorithm::save(const string& file_name) const
    {
        try {
            FILE* file = fopen(file_name.c_str(), "w");

            if (file)
            {
                tinyxml2::XMLPrinter printer(file);
                write_XML(printer);
                fclose(file);
            }

        } catch (exception e) {
            cout<< e.what();
        }

    }


    /// Loads a genetic algorithm object from an XML-type file.
    /// @param file_name Name of genetic algorithm XML-type file.

    void GeneticAlgorithm::load(const string& file_name)
    {
        set_default();

        tinyxml2::XMLDocument document;

        if (document.LoadFile(file_name.c_str()))
        {
            ostringstream buffer;

            buffer << "OpenNN Exception: GeneticAlgorithm class.\n"
                << "void load(const string&) method.\n"
                << "Cannot load XML file " << file_name << ".\n";

            throw invalid_argument(buffer.str());
        }

        from_XML(document);
    }




}

// OpenNN: Open Neural Networks Library.
// Copyright(C) 2005-2021 Artificial Intelligence Techniques, SL.
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

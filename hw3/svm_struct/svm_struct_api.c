/***********************************************************************/
/*                                                                     */
/*   svm_struct_api.c                                                  */
/*                                                                     */
/*   Definition of API for attaching implementing SVM learning of      */
/*   structures (e.g. parsing, multi-label classification, HMM)        */ 
/*                                                                     */
/*   Author: Thorsten Joachims                                         */
/*   Date: 03.07.04                                                    */
/*                                                                     */
/*   Copyright (c) 2004  Thorsten Joachims - All rights reserved       */
/*                                                                     */
/*   This software is available for non-commercial use only. It must   */
/*   not be modified and distributed without prior permission of the   */
/*   author. The author is not responsible for implications from the   */
/*   use of this software.                                             */
/*                                                                     */
/***********************************************************************/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "svm_struct/svm_struct_common.h"
#include "svm_struct_api.h"
#include "svm_struct/svm_struct_learn.h"

void        svm_struct_learn_api_init(int argc, char* argv[])
{
  /* Called in learning part before anything else is done to allow
     any initializations that might be necessary. */
}

void        svm_struct_learn_api_exit()
{
  /* Called in learning part at the very end to allow any clean-up
     that might be necessary. */
}

void        svm_struct_classify_api_init(int argc, char* argv[])
{
  /* Called in prediction part before anything else is done to allow
     any initializations that might be necessary. */
}

void        svm_struct_classify_api_exit()
{
  /* Called in prediction part at the very end to allow any clean-up
     that might be necessary. */
}

SAMPLE      read_struct_examples(char *file, STRUCT_LEARN_PARM *sparm)
{
  /* Reads struct examples and returns them in sample. The number of
     examples must be written into sample.n */
  SAMPLE   sample;  /* sample */
  EXAMPLE  *examples;
  long     n;       /* number of examples */
  n = TRAIN_EXAMPLES; /* replace by appropriate number of examples */
  examples=(EXAMPLE *)my_malloc(sizeof(EXAMPLE)*n);

  /* fill in your code here */
  double (*x)[48];
  int *y;
  double tmp_x[MAX_SEQ_LEN][48];
  int tmp_y[MAX_SEQ_LEN];
  char tmp[3000];
  char *tmp_tok;
  int tmp_tmp_y;
  int c, i, j, k;
  FILE *fp;
  fp = fopen(file, "r");
  int count = 0;
  int current_c = 0;
  while(fgets(tmp, 3000, fp) != NULL){
    tmp_tok = strtok(tmp, " \n");
    tmp_tmp_y = atoi(tmp_tok);
    tmp_tok = strtok(NULL, " \n");
    c = atoi(tmp_tok);
    if(c != current_c){
      x = malloc(48*count*sizeof(double));
      y = (int *)malloc(sizeof(int) * count);
      for(j = 0; j < count; j++){
        for(k = 0; k < 48; k++){
          x[j][k] = tmp_x[j][k];  
        }
        y[j] = tmp_y[j];
      }
      examples[current_c].x.x = x;
      examples[current_c].x.len = count;
      examples[current_c].y.y = y;
      examples[current_c].y.len = count;
      count = 0;
      current_c++;
    }
    tmp_y[count] = tmp_tmp_y;
    for(i = 0; i < 48; i++){
      tmp_tok = strtok(NULL, " \n");
      tmp_x[count][i] = atof(tmp_tok);
    }
    /*
    for(i = 0; i < 48; i++){
      printf("#%d : %.20lf\n", i, x[i]);
    }
    */
    //printf("y = %d\nc = %d\n", y, c);
    count++;
  }
  x = malloc(48*count*sizeof(double));
  y = (int *)malloc(sizeof(int) * count);
  for(j = 0; j < count; j++){
    for( k = 0; k < 48; k++){
      x[j][k] = tmp_x[j][k];  
    }
    y[j] = tmp_y[j];
    
  }
  examples[current_c].x.x = x;
  examples[current_c].x.len = count;
  examples[current_c].y.y = y;
  examples[current_c].y.len = count;


  sample.n=n;
  sample.examples=examples;
  fclose(fp);
  return(sample);
}

void        init_struct_model(SAMPLE sample, STRUCTMODEL *sm, 
			      STRUCT_LEARN_PARM *sparm, LEARN_PARM *lparm, 
			      KERNEL_PARM *kparm)
{
  /* Initialize structmodel sm. The weight vector w does not need to be
     initialized, but you need to provide the maximum size of the
     feature space in sizePsi. This is the maximum number of different
     weights that can be learned. Later, the weight vector w will
     contain the learned weights for the model. */

  sm->sizePsi=48*48+48*48; /* replace by appropriate number of features */
}

CONSTSET    init_struct_constraints(SAMPLE sample, STRUCTMODEL *sm, 
				    STRUCT_LEARN_PARM *sparm)
{
  /* Initializes the optimization problem. Typically, you do not need
     to change this function, since you want to start with an empty
     set of constraints. However, if for example you have constraints
     that certain weights need to be positive, you might put that in
     here. The constraints are represented as lhs[i]*w >= rhs[i]. lhs
     is an array of feature vectors, rhs is an array of doubles. m is
     the number of constraints. The function returns the initial
     set of constraints. */
  CONSTSET c;
  long     sizePsi=sm->sizePsi;
  long     i;
  WORD     words[2];

  if(1) { /* normal case: start with empty set of constraints */
    c.lhs=NULL;
    c.rhs=NULL;
    c.m=0;
  }
  else { /* add constraints so that all learned weights are
            positive. WARNING: Currently, they are positive only up to
            precision epsilon set by -e. */
    c.lhs=my_malloc(sizeof(DOC *)*sizePsi);
    c.rhs=my_malloc(sizeof(double)*sizePsi);
    for(i=0; i<sizePsi; i++) {
      words[0].wnum=i+1;
      words[0].weight=1.0;
      words[1].wnum=0;
      /* the following slackid is a hack. we will run into problems,
         if we have move than 1000000 slack sets (ie examples) */
      c.lhs[i]=create_example(i,0,1000000+i,1,create_svector(words,"",1.0));
      c.rhs[i]=0.0;
    }
  }
  return(c);
}

LABEL       classify_struct_example(PATTERN x, STRUCTMODEL *sm, 
				    STRUCT_LEARN_PARM *sparm)
{
  /* Finds the label yhat for pattern x that scores the highest
     according to the linear evaluation function in sm, especially the
     weights sm.w. The returned label is taken as the prediction of sm
     for the pattern x. The weights correspond to the features defined
     by psi() and range from index 1 to index sm->sizePsi. If the
     function cannot find a label, it shall return an empty label as
     recognized by the function empty_label(y). */
  LABEL y_hat;
  int FrameNum = x.len;
  y_hat.len = FrameNum;
  double W[48][48];
  double W_trans[48][48];
  double v_matrix[48][FrameNum];
  int trace[48][FrameNum];

  int i, j, k;
  for(i = 0; i < 48; i++){
    v_matrix[i][0] = 0;
    for(j = 0; j < 48; j++){
      W[i][j] = sm->w[ i * 48 + j + 1 ];
      v_matrix[i][0] += x.x[0][j] * W[i][j];
      W_trans[j][i] = sm->w[ 2305 + i * 48 + j];
    }
  }
  double tmp_prob;
  for(i = 1; i < FrameNum; i++){
    double max_prob[48] = {0};
    for(j = 0; j < 48; j++){
      for(k = 0; k < 48; k++){
        tmp_prob = v_matrix[j][i-1] + W_trans[k][j]; //P(j) * P(j -> k)
        if(max_prob[k] < tmp_prob){
          max_prob[k] = tmp_prob;
          trace[k][i] = j;
        }
      }
    }
    for(j = 0; j < 48; j++){
      v_matrix[j][i] = max_prob[j];
      for(k = 0; k < 48; k++){
        v_matrix[j][i] += x.x[i][k] * W[j][k];
      }
    }
  }

  int max_final_prob = v_matrix[0][FrameNum-1];
  int max_final_prob_index = 0;
  for(i = 1; i < 48; i++){
    if(max_final_prob < v_matrix[i][FrameNum-1]){
      max_final_prob = v_matrix[i][FrameNum-1];
      max_final_prob_index = i;
    }
  }

  y_hat.y = (int *)malloc(sizeof(int) * FrameNum);
  y_hat.y[FrameNum-1] = max_final_prob_index;
  for(int i = FrameNum-2; i >=0; i--){
    y_hat.y[i] = trace[y_hat.y[i+1]][i+1];
  }

  return(y_hat);
}

LABEL       find_most_violated_constraint_slackrescaling(PATTERN x, LABEL y, 
						     STRUCTMODEL *sm, 
						     STRUCT_LEARN_PARM *sparm)
{
  /* Finds the label ybar for pattern x that that is responsible for
     the most violated constraint for the slack rescaling
     formulation. For linear slack variables, this is that label ybar
     that maximizes

            argmax_{ybar} loss(y,ybar)*(1-psi(x,y)+psi(x,ybar)) 

     Note that ybar may be equal to y (i.e. the max is 0), which is
     different from the algorithms described in
     [Tschantaridis/05]. Note that this argmax has to take into
     account the scoring function in sm, especially the weights sm.w,
     as well as the loss function, and whether linear or quadratic
     slacks are used. The weights in sm.w correspond to the features
     defined by psi() and range from index 1 to index
     sm->sizePsi. Most simple is the case of the zero/one loss
     function. For the zero/one loss, this function should return the
     highest scoring label ybar (which may be equal to the correct
     label y), or the second highest scoring label ybar, if
     Psi(x,ybar)>Psi(x,y)-1. If the function cannot find a label, it
     shall return an empty label as recognized by the function
     empty_label(y). */
  LABEL ybar;

  /* insert your code for computing the label ybar here */

  return(ybar);
}


LABEL       find_most_violated_constraint_marginrescaling(PATTERN x, LABEL y, 
						     STRUCTMODEL *sm, 
						     STRUCT_LEARN_PARM *sparm)
{
  /* Finds the label ybar for pattern x that that is responsible for
     the most violated constraint for the margin rescaling
     formulation. For linear slack variables, this is that label ybar
     that maximizes

            argmax_{ybar} loss(y,ybar)+psi(x,ybar)

     Note that ybar may be equal to y (i.e. the max is 0), which is
     different from the algorithms described in
     [Tschantaridis/05]. Note that this argmax has to take into
     account the scoring function in sm, especially the weights sm.w,
     as well as the loss function, and whether linear or quadratic
     slacks are used. The weights in sm.w correspond to the features
     defined by psi() and range from index 1 to index
     sm->sizePsi. Most simple is the case of the zero/one loss
     function. For the zero/one loss, this function should return the
     highest scoring label ybar (which may be equal to the correct
     label y), or the second highest scoring label ybar, if
     Psi(x,ybar)>Psi(x,y)-1. If the function cannot find a label, it
     shall return an empty label as recognized by the function
     empty_label(y). */
  LABEL y_bar;
  int FrameNum = x.len;
  y_bar.len = FrameNum;
  double W[48][48];
  double W_trans[48][48];
  double v_matrix[48][FrameNum];
  int trace[48][FrameNum];

  int i, j, k;

  double loss_matrix[48][FrameNum];
  for(i = 0; i < FrameNum; i++){
    for(j = 0; j < 48; j++){
      loss_matrix[j][i] = (double)1/FrameNum;
    }
    loss_matrix[y.y[i]][i] = 0;
  }

  for(i = 0; i < 48; i++){
    v_matrix[i][0] = loss_matrix[i][0];
    for(j = 0; j < 48; j++){
      W[i][j] = sm->w[ i * 48 + j + 1 ];
      v_matrix[i][0] += x.x[0][j] * W[i][j];
      W_trans[j][i] = sm->w[ 2305 + i * 48 + j];
    }
  }

  double tmp_prob;
  for(i = 1; i < FrameNum; i++){
    double max_prob[48] = {0};
    for(j = 0; j < 48; j++){
      for(k = 0; k < 48; k++){
        tmp_prob = v_matrix[j][i-1] + W_trans[k][j]; //P(j) * P(j -> k)
        if(max_prob[k] < tmp_prob){
          max_prob[k] = tmp_prob;
          trace[k][i] = j;
        }
      }
    }
    for(j = 0; j < 48; j++){
      v_matrix[j][i] = max_prob[j] + loss_matrix[j][i];
      for(k = 0; k < 48; k++){
        v_matrix[j][i] += x.x[i][k] * W[j][k];
      }
    }
  }
  
  double max_final_prob = v_matrix[0][FrameNum-1];
  int max_final_prob_index = 0;
  for(i = 1; i < 48; i++){
    if(max_final_prob < v_matrix[i][FrameNum-1]){
      max_final_prob = v_matrix[i][FrameNum-1];
      max_final_prob_index = i;
    }
  }

  y_bar.y = (int *)malloc(sizeof(int) * FrameNum);
  y_bar.y[FrameNum-1] = max_final_prob_index;
  for(i = FrameNum-2; i >= 0; i--){
    y_bar.y[i] = trace[y_bar.y[i+1]][i+1];
  }


  /*//
  printf("\n=============================\n");
  for(i = 0; i < 48; i++){
    for(j = 25; j < 30; j++){
      printf("%.15lf ", W[j][i]);
    }
    printf("\n");
  }
  printf("=============================\n");

  ///
  for(i = 0; i < FrameNum; i++){
    printf("%d ", y_bar.y[i]);
  }
  printf("\n")
  //*/


  return(y_bar);
}

int         empty_label(LABEL y)
{
  /* Returns true, if y is an empty label. An empty label might be
     returned by find_most_violated_constraint_???(x, y, sm) if there
     is no incorrect label that can be found for x, or if it is unable
     to label x at all */
  return(0);
}

SVECTOR     *psi(PATTERN x, LABEL y, STRUCTMODEL *sm,
		 STRUCT_LEARN_PARM *sparm)
{
  /* Returns a feature vector describing the match between pattern x
     and label y. The feature vector is returned as a list of
     SVECTOR's. Each SVECTOR is in a sparse representation of pairs
     <featurenumber:featurevalue>, where the last pair has
     featurenumber 0 as a terminator. Featurenumbers start with 1 and
     end with sizePsi. Featuresnumbers that are not specified default
     to value 0. As mentioned before, psi() actually returns a list of
     SVECTOR's. Each SVECTOR has a field 'factor' and 'next'. 'next'
     specifies the next element in the list, terminated by a NULL
     pointer. The list can be though of as a linear combination of
     vectors, where each vector is weighted by its 'factor'. This
     linear combination of feature vectors is multiplied with the
     learned (kernelized) weight vector to score label y for pattern
     x. Without kernels, there will be one weight in sm.w for each
     feature. Note that psi has to match
     find_most_violated_constraint_???(x, y, sm) and vice versa. In
     particular, find_most_violated_constraint_???(x, y, sm) finds
     that ybar!=y that maximizes psi(x,ybar,sm)*sm.w (where * is the
     inner vector product) and the appropriate function of the
     loss + margin/slack rescaling method. See that paper for details. */
  SVECTOR *fvec = (SVECTOR *)malloc(sizeof(SVECTOR));
  int FrameNum = x.len;

  double Matrix[48][48];
  double TrainsitionMatrix[48][48];
  int i, j;

  for(i = 0; i < 48; i++){
    for(j = 0; j < 48; j++){
      Matrix[i][j] = 0;
      TrainsitionMatrix[i][j] = 0;
    }
  }

  for(j = 0; j < 48; j++){
    Matrix[y.y[0]][j] += x.x[0][j];
  }

  for(i = 1; i < FrameNum; i++){
    for(j = 0; j < 48; j++){
      Matrix[y.y[i]][j] += x.x[i][j];
    }
    TrainsitionMatrix[y.y[i-1]][y.y[i]] += 1;
  }

  WORD tmp_word[5000];
  int nonzero = 0;

  for(i = 0; i < 48; i++){
    for(j = 0; j < 48; j++){
      if(Matrix[i][j] != 0){
        tmp_word[nonzero].wnum = i * 48 + j + 1;
        tmp_word[nonzero].weight = Matrix[i][j];
        nonzero ++;
      }
    }
  }

  for(i = 0; i < 48; i++){
    for(j = 0; j < 48; j++){
      if(TrainsitionMatrix[i][j] != 0){
        tmp_word[nonzero].wnum = 2304 + i * 48 + j + 1;
        tmp_word[nonzero].weight = TrainsitionMatrix[i][j];
        nonzero ++;
      }
    }
  }
  tmp_word[nonzero].wnum = 0;

  fvec->words = (WORD *)malloc(sizeof(WORD) * (nonzero + 1) );
  for(i = 0; i <= nonzero; i++){
    fvec->words[i] = tmp_word[i];
  }
  fvec->next = NULL;
  fvec->userdefined = NULL;
  fvec->factor = 1;
  fvec->kernel_id = 0;

  /*
  for(i = 0; i <= nonzero; i++){
    printf("%d:%lf\n", fvec->words[i].wnum, fvec->words[i].weight);
  }
  */

  return(fvec);
}

double      loss(LABEL y, LABEL ybar, STRUCT_LEARN_PARM *sparm)
{
  /* loss for correct label y and predicted label ybar. The loss for
     y==ybar has to be zero. sparm->loss_function is set with the -l option. */
  int i;
  /* type 0 loss: 0/1 loss */
  /* return 0, if y==ybar. return 1 else */
  /*

  if(sparm->loss_function == 0) {
    for(i = 0; i < y.len; i++){
      if(y.y[i] != ybar.y[i]){
        return 1;
      }
    }
    return 0;
  }
  else {
    double Loss = 0;
    for(i = 0; i < y.len; i++){
      if(y.y[i] != ybar.y[i]){
        Loss++;
      }
    }
    return Loss/y.len;
  }
  */
  double Loss = 0;
  for(i = 0; i < y.len; i++){
    if(y.y[i] != ybar.y[i]){
      Loss++;
    }
  }
  return Loss/y.len;
}

int         finalize_iteration(double ceps, int cached_constraint,
			       SAMPLE sample, STRUCTMODEL *sm,
			       CONSTSET cset, double *alpha, 
			       STRUCT_LEARN_PARM *sparm)
{
  /* This function is called just before the end of each cutting plane iteration. ceps is the amount by which the most violated constraint found in the current iteration was violated. cached_constraint is true if the added constraint was constructed from the cache. If the return value is FALSE, then the algorithm is allowed to terminate. If it is TRUE, the algorithm will keep iterating even if the desired precision sparm->epsilon is already reached. */
  return(0);
}

void        print_struct_learning_stats(SAMPLE sample, STRUCTMODEL *sm,
					CONSTSET cset, double *alpha, 
					STRUCT_LEARN_PARM *sparm)
{
  /* This function is called after training and allows final touches to
     the model sm. But primarly it allows computing and printing any
     kind of statistic (e.g. training error) you might want. */
}

void        print_struct_testing_stats(SAMPLE sample, STRUCTMODEL *sm,
				       STRUCT_LEARN_PARM *sparm, 
				       STRUCT_TEST_STATS *teststats)
{
  /* This function is called after making all test predictions in
     svm_struct_classify and allows computing and printing any kind of
     evaluation (e.g. precision/recall) you might want. You can use
     the function eval_prediction to accumulate the necessary
     statistics for each prediction. */
}

void        eval_prediction(long exnum, EXAMPLE ex, LABEL ypred, 
			    STRUCTMODEL *sm, STRUCT_LEARN_PARM *sparm, 
			    STRUCT_TEST_STATS *teststats)
{
  /* This function allows you to accumlate statistic for how well the
     predicition matches the labeled example. It is called from
     svm_struct_classify. See also the function
     print_struct_testing_stats. */
  if(exnum == 0) { /* this is the first time the function is
		      called. So initialize the teststats */
  }
}

void        write_struct_model(char *file, STRUCTMODEL *sm, 
			       STRUCT_LEARN_PARM *sparm)
{
  /* Writes structural model sm to file file. */
  write_model(file,sm->svm_model);
}

STRUCTMODEL read_struct_model(char *file, STRUCT_LEARN_PARM *sparm)
{
  /* Reads structural model sm from file file. This function is used
     only in the prediction module, not in the learning module. */
  STRUCTMODEL sm;
  sm.svm_model=read_model(file);
  sm.w=sm.svm_model->lin_weights;
  sm.sizePsi=sm.svm_model->totwords;
  return(sm);
}

void        write_label(FILE *fp, LABEL y)
{
  /* Writes label y to file handle fp. */
  int i;
  for(i = 0; i < y.len; i++){
    fprintf(fp, "%d\n", y.y[i]);
  }
} 

void        free_pattern(PATTERN x) {
  /* Frees the memory of x. */
  free(x.x);
}

void        free_label(LABEL y) {
  /* Frees the memory of y. */
  free(y.y);
}

void        free_struct_model(STRUCTMODEL sm) 
{
  /* Frees the memory of model. */
  /* if(sm.w) free(sm.w); */ /* this is free'd in free_model */
  if(sm.svm_model) free_model(sm.svm_model,1);
  /* add free calls for user defined data here */
}

void        free_struct_sample(SAMPLE s)
{
  /* Frees the memory of sample s. */
  int i;
  for(i=0;i<s.n;i++) {
    free_pattern(s.examples[i].x);
    free_label(s.examples[i].y);
  }
  free(s.examples);
}

void        print_struct_help()
{
  /* Prints a help text that is appended to the common help text of
     svm_struct_learn. */
  printf("         --* string  -> custom parameters that can be adapted for struct\n");
  printf("                        learning. The * can be replaced by any character\n");
  printf("                        and there can be multiple options starting with --.\n");
}

void         parse_struct_parameters(STRUCT_LEARN_PARM *sparm)
{
  /* Parses the command line parameters that start with -- */
  int i;

  for(i=0;(i<sparm->custom_argc) && ((sparm->custom_argv[i])[0] == '-');i++) {
    switch ((sparm->custom_argv[i])[2]) 
      { 
      case 'a': i++; /* strcpy(learn_parm->alphafile,argv[i]); */ break;
      case 'e': i++; /* sparm->epsilon=atof(sparm->custom_argv[i]); */ break;
      case 'k': i++; /* sparm->newconstretrain=atol(sparm->custom_argv[i]); */ break;
      default: printf("\nUnrecognized option %s!\n\n",sparm->custom_argv[i]);
	       exit(0);
      }
  }
}

void        print_struct_help_classify()
{
  /* Prints a help text that is appended to the common help text of
     svm_struct_classify. */
  printf("         --* string -> custom parameters that can be adapted for struct\n");
  printf("                       learning. The * can be replaced by any character\n");
  printf("                       and there can be multiple options starting with --.\n");
}

void         parse_struct_parameters_classify(STRUCT_LEARN_PARM *sparm)
{
  /* Parses the command line parameters that start with -- for the
     classification module */
  int i;

  for(i=0;(i<sparm->custom_argc) && ((sparm->custom_argv[i])[0] == '-');i++) {
    switch ((sparm->custom_argv[i])[2]) 
      { 
      /* case 'x': i++; strcpy(xvalue,sparm->custom_argv[i]); break; */
      default: printf("\nUnrecognized option %s!\n\n",sparm->custom_argv[i]);
	       exit(0);
      }
  }
}


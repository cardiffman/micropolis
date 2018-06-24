/*
 * w_graph.h
 *
 *  Created on: Jun 3, 2018
 *      Author: Mike
 */

#ifndef SRC_SIM_HEADERS_W_GRAPH_H_
#define SRC_SIM_HEADERS_W_GRAPH_H_


extern short NewGraph;
extern short Graph10Max, Graph120Max;

void initGraphs(void);
void graphDoer(void);
void InitGraphMax(void);
void doAllGraphs(void);
void ChangeCensus(void);
typedef struct SimGraph SimGraph;
void DestroyGraph(SimGraph *graph);
void graph_command_init(void);



#endif /* SRC_SIM_HEADERS_W_GRAPH_H_ */

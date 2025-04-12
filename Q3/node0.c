#include <stdio.h>

extern struct rtpkt {
  int sourceid;       /* id of sending router sending this pkt */
  int destid;         /* id of router to which pkt being sent 
                         (must be an immediate neighbor) */
  int mincost[4];    /* min cost to node 0 ... 3 */
  };

extern int TRACE; // TRACE is a global variable that controls the level of debugging output
extern int YES; 
extern int NO;
extern float clocktime; // Current simulation time

// int connectcosts0[4] = { 0,  1,  3, 7 };

struct distance_table 
{
  int costs[4][4];
} dt0;

/* students to write the following two routines, and maybe some others */

int node0_mincost[4]; // Minimum cost to each node from node 0

void rtinit0() 
{
    printf("\n=== At time t=%.3f, rtinit0() called ===\n", clocktime);
    int i, j;
    
    // Initialize distance table with infinity (~999)
    for(i = 0; i < 4; i++)
        for(j = 0; j < 4; j++)
            dt0.costs[i][j] = 999;
    
    // Initialize direct costs
    dt0.costs[0][0] = 0;
    dt0.costs[1][1] = 1;    // Cost to node 1 
    dt0.costs[2][2] = 3;    // Cost to node 2
    dt0.costs[3][3] = 7;    // Cost to node 3
    
 
    // Send initial distance vector to neighbors
    struct rtpkt packet;
    packet.sourceid = 0; // Source ID is 0 (this node)
    
    for(i = 0; i < 4; i++) {
        packet.mincost[i] = dt0.costs[i][i]; // Minimum cost to each node is updated in the packet
        node0_mincost[i] = dt0.costs[i][i]; // Store minimum costs for node 0
    }


    printf("Initial distance table for node 0:\n");
    printdt0(&dt0); // Print initial distance table
    
    printf("Node 0 sending routing packets to neighbors (1, 2, 3):\n");
    printf("  mincost: %d %d %d %d\n", packet.mincost[0], packet.mincost[1], 
           packet.mincost[2], packet.mincost[3]);
    
  
    // Send to each neighbor
    packet.destid = 1; // Send to node 1
    tolayer2(packet);
    packet.destid = 2; // Send to node 2
    tolayer2(packet);
    packet.destid = 3; // Send to node 3
    tolayer2(packet);
}


void rtupdate0(rcvdpkt)
  struct rtpkt *rcvdpkt;
{
    printf("\n=== At time t=%.3f, rtupdate0() called, received routing packet from Node %d ===\n", 
           clocktime, rcvdpkt->sourceid);
    printf("Received mincost: %d %d %d %d\n", 
           rcvdpkt->mincost[0], rcvdpkt->mincost[1], 
           rcvdpkt->mincost[2], rcvdpkt->mincost[3]);
    
    int src = rcvdpkt->sourceid; // Source ID of the received packet

    int i, j;
    
    int dt_changed = 0; // Flag to check if distance table changed
    
    // Update distance table based on received packet
    for(i = 0; i < 4; i++) {
        // Check if the cost to reach node i through the source node is less than the current cost
        // If yes, update the distance table
        if(dt0.costs[i][src] > rcvdpkt->mincost[i] + dt0.costs[src][src]) {
            dt0.costs[i][src] = rcvdpkt->mincost[i] + dt0.costs[src][src];
            dt_changed = 1; // Distance table changed
        }
    }

    if (dt_changed) {
        printf("Distance table updated for node 0\n");
    } else {
        printf("Distance table NOT updated for node 0\n");
    }

    
    int prev_mincost[4];
    memcpy(prev_mincost, node0_mincost, sizeof(node0_mincost)); // Store previous minimum costs
    

    // Calculate minimum costs
    for(i = 0; i < 4; i++) {
      int min = 999;
      for(j = 0; j < 4; j++) {
          if(dt0.costs[i][j] < min)
              min = dt0.costs[i][j];
      }
      node0_mincost[i] = min; // Update minimum costs for node 0
    }

    int changed = 0; // Flag to check if minimum costs changed
    for(int i=0;i<4;i++){
        if(prev_mincost[i]!=node0_mincost[i]){
            changed = 1; // Minimum costs changed
        }
    }

    // If distance table changed, send updates to neighbors
    if(changed) {
        struct rtpkt packet; // Create a new packet to send updates
        packet.sourceid = 0;
        for (i = 0; i < 4; i++)
            packet.mincost[i] = node0_mincost[i]; // Update minimum costs in the packet
        
        printf("\nNode 0 distance table updated to:\n");
        printdt0(&dt0); // Print updated distance table
        printf("Node 0 sending routing packets to neighbors with mincost: %d %d %d %d\n",
               packet.mincost[0], packet.mincost[1], 
               packet.mincost[2], packet.mincost[3]); // Print updated minimum costs
        printf("\n");

        // Send to each neighbor
        packet.destid = 1; // Send to node 1
        tolayer2(packet);
        packet.destid = 2; // Send to node 2
        tolayer2(packet);
        packet.destid = 3; // Send to node 3
        tolayer2(packet);
    } else {
        printf("Minimum costs NOT updated\n");
        printf("Current distance table for node 0:\n");
        printdt0(&dt0); // Print current distance table
    }
}

printdt0(dtptr)
  struct distance_table *dtptr;
{
 
  printf("                via     \n");
  printf("   D0 |    1     2    3 \n");
  printf("  ----|-----------------\n");
  printf("     1|  %3d   %3d   %3d\n",dtptr->costs[1][1],
	 dtptr->costs[1][2],dtptr->costs[1][3]);
  printf("dest 2|  %3d   %3d   %3d\n",dtptr->costs[2][1],
	 dtptr->costs[2][2],dtptr->costs[2][3]);
  printf("     3|  %3d   %3d   %3d\n",dtptr->costs[3][1],
	 dtptr->costs[3][2],dtptr->costs[3][3]);
  printf("\n");

}

linkhandler0(linkid, newcost)   
  int linkid, newcost;

/* called when cost from 0 to linkid changes from current value to newcost*/
/* You can leave this routine empty if you're an undergrad. If you want */
/* to use this routine, you'll need to change the value of the LINKCHANGE */
/* constant definition in prog3.c from 0 to 1 */
	
{
}


// References
// 1. Distance Vector Routing Algorithm: https://en.wikipedia.org/wiki/Distance-vector_routing_protocol
// 2. Computer Networking: A Top-Down Approach by Kurose and Ross
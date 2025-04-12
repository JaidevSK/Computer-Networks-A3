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

// int connectcosts1[4] = { 1,  0,  1, 999 };

struct distance_table
{
  int costs[4][4];
} dt1;

/* students to write the following two routines, and maybe some others */

int node1_mincost[4]; // Minimum cost to each node from node 1

void rtinit1() 
{
    printf("\n=== At time t=%.3f, rtinit1() called ===\n", clocktime);
    int i, j;
    
    // Initialize distance table with infinity (~999)
    for(i = 0; i < 4; i++)
        for(j = 0; j < 4; j++)
            dt1.costs[i][j] = 999;
    
    // Initialize direct costs
    dt1.costs[0][0] = 1;    // Cost to node 0
    dt1.costs[1][1] = 0;    // Cost to self (node 1)
    dt1.costs[2][2] = 1;    // Cost to node 2
    dt1.costs[3][3] = 999;    // Cost to node 3
    
    // Send initial distance vector to neighbors
    struct rtpkt packet;
    packet.sourceid = 1; // Source ID is 1 (this node)
    
    for(i = 0; i < 4; i++) {
        packet.mincost[i] = dt1.costs[i][i]; // Minimum cost to each node is updated in the packet
        node1_mincost[i] = dt1.costs[i][i]; // Store minimum costs for node 1
    }

    printf("Initial distance table for node 1:\n");
    printdt1(&dt1);
    
    printf("Node 1 sending routing packets to neighbors (0, 2):\n");
    printf("  mincost: %d %d %d %d\n", packet.mincost[0], packet.mincost[1], 
           packet.mincost[2], packet.mincost[3]);
    
    // Send to each neighbor
    packet.destid = 0;
    tolayer2(packet);
    packet.destid = 2;
    tolayer2(packet);
    packet.destid = 3;
    tolayer2(packet);
}

void rtupdate1(rcvdpkt)
  struct rtpkt *rcvdpkt;
{
    printf("\n=== At time t=%.3f, rtupdate1() called, received routing packet from Node %d ===\n", 
           clocktime, rcvdpkt->sourceid);
    printf("Received mincost: %d %d %d %d\n", 
           rcvdpkt->mincost[0], rcvdpkt->mincost[1], 
           rcvdpkt->mincost[2], rcvdpkt->mincost[3]);

    int i, j;
    int src = rcvdpkt->sourceid; // Source ID of the received packet

    int dt_changed = 0; // Flag to check if distance table changed
    
    // Update distance table based on received packet
    for(i = 0; i < 4; i++) {
        // Check if the cost to node i through the received packet is less than the current cost
        // If so, update the distance table
        if(dt1.costs[i][src] > rcvdpkt->mincost[i] + dt1.costs[src][src]) {
            dt1.costs[i][src] = rcvdpkt->mincost[i] + dt1.costs[src][src];
            dt_changed = 1; // Distance table changed
        } 
    }

    if (dt_changed) {
        printf("Distance table updated for node 1\n");
    } else {
        printf("Distance table NOT updated for node 1\n");
    }

    int prev_mincost[4];
    memcpy(prev_mincost, node1_mincost, sizeof(node1_mincost)); // Store previous minimum costs
    

    // Calculate minimum costs
    for(i = 0; i < 4; i++) {
      int min = 999;
      for(j = 0; j < 4; j++) {
          if(dt1.costs[i][j] < min)
              min = dt1.costs[i][j]; // Find minimum cost to node i
      }
      node1_mincost[i] = min; // Update minimum costs for node 1
    }

    int changed = 0; // Flag to check if minimum costs changed
    for(int i=0;i<4;i++){
        if(prev_mincost[i]!=node1_mincost[i]){
            changed = 1; // Minimum costs changed
        }
    }
    
    // If distance table changed, send updates to neighbors
    if(changed) {
        struct rtpkt packet; // Create a new packet to send updates
        packet.sourceid = 1;
        for (i = 0; i < 4; i++)
            packet.mincost[i] = node1_mincost[i];

        printf("\nNode 1 distance table updated to:\n");
        printdt1(&dt1);
        printf("Node 1 sending routing packets to neighbors with mincost: %d %d %d %d\n",
                packet.mincost[0], packet.mincost[1], 
                packet.mincost[2], packet.mincost[3]); // Print updated minimum costs
        printf("\n");

        // Send to each neighbor
        packet.destid = 0; // Send to node 0
        tolayer2(packet);
        packet.destid = 2; // Send to node 2
        tolayer2(packet);
        packet.destid = 3; // Send to node 3
        tolayer2(packet);
        
        
    } else {
        printf("Minimum costs NOT updated\n");
        printf("Current distance table for node 1:\n");
        printdt1(&dt1); // Print current distance table
    }
}

printdt1(dtptr)
  struct distance_table *dtptr;
  
{
  
  printf("             via   \n");
  printf("   D1 |    0     2 \n");
  printf("  ----|-----------\n");
  printf("     0|  %3d   %3d\n",dtptr->costs[0][0], dtptr->costs[0][2]);
  printf("dest 2|  %3d   %3d\n",dtptr->costs[2][0], dtptr->costs[2][2]);
  printf("     3|  %3d   %3d\n",dtptr->costs[3][0], dtptr->costs[3][2]);
  printf("\n");
}



linkhandler1(linkid, newcost)   
int linkid, newcost;   


/* called when cost from 1 to linkid changes from current value to newcost*/
/* You can leave this routine empty if you're an undergrad. If you want */
/* to use this routine, you'll need to change the value of the LINKCHANGE */
/* constant definition in prog3.c from 0 to 1 */
	
{   
}


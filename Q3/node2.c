#include <stdio.h>

extern struct rtpkt {
  int sourceid;       /* id of sending router sending this pkt */
  int destid;         /* id of router to which pkt being sent 
                         (must be an immediate neighbor) */
  int mincost[4];    /* min cost to node 0 ... 3 */
  };

extern int TRACE;
extern int YES;
extern int NO;
extern float clocktime;

// int connectcosts2[4] = { 3,  1,  0, 2 };

struct distance_table 
{
  int costs[4][4];
} dt2;


/* students to write the following two routines, and maybe some others */

int node2_mincost[4];

void rtinit2() 
{
    printf("\n=== At time t=%.3f, rtinit2() called ===\n", clocktime);
    int i, j;
    
    // Initialize distance table with infinity
    for(i = 0; i < 4; i++)
        for(j = 0; j < 4; j++)
            dt2.costs[i][j] = 999;
    
    // Initialize direct costs
    dt2.costs[0][0] = 3;    // Cost to node 0
    dt2.costs[1][1] = 1;    // Cost to node 1
    dt2.costs[2][2] = 0;    // Cost to self (node 2)
    dt2.costs[3][3] = 2;    // Cost to node 3
    

    // Send initial distance vector to neighbors
    struct rtpkt packet;
    packet.sourceid = 2;
    
    for(i = 0; i < 4; i++) {
        packet.mincost[i] = dt2.costs[i][i];
        node2_mincost[i] = dt2.costs[i][i];
    }

    printf("Initial distance table for node 2:\n");
    printdt2(&dt2);
    
    printf("Node 2 sending routing packets to neighbors (0, 1, 3):\n");
    printf("  mincost: %d %d %d %d\n", packet.mincost[0], packet.mincost[1], 
           packet.mincost[2], packet.mincost[3]);
    
    // Send to each neighbor
    packet.destid = 0;
    tolayer2(packet);
    packet.destid = 1;
    tolayer2(packet);
    packet.destid = 3;
    tolayer2(packet);
}

void rtupdate2(rcvdpkt)
  struct rtpkt *rcvdpkt;
{
    printf("\n=== At time t=%.3f, rtupdate2() called, received routing packet from Node %d ===\n", 
           clocktime, rcvdpkt->sourceid);
    printf("Received mincost: %d %d %d %d\n", 
           rcvdpkt->mincost[0], rcvdpkt->mincost[1], 
           rcvdpkt->mincost[2], rcvdpkt->mincost[3]);

    int i, j;

    int src = rcvdpkt->sourceid;
    int dest = rcvdpkt->destid;
    
    int dt_changed = 0;

    // Update distance table based on received packet
    for(i = 0; i < 4; i++) {
        if(dt2.costs[i][src] > rcvdpkt->mincost[i] + dt2.costs[src][src]) {
            dt2.costs[i][src] = rcvdpkt->mincost[i] + dt2.costs[src][src];
            dt_changed = 1;
        }
    }

    if (dt_changed) {
        printf("Distance table updated for node 2\n");
    } else {
        printf("Distance table NOT updated for node 2\n");
    }
    
    int prev_mincost[4];
    memcpy(prev_mincost, node2_mincost, sizeof(node2_mincost));
    

    // Calculate minimum costs
    for(i = 0; i < 4; i++) {
      int min = 999;
      for(j = 0; j < 4; j++) {
          if(dt2.costs[i][j] < min)
              min = dt2.costs[i][j];
      }
      node2_mincost[i] = min;
    }

    int changed = 0;
    for(int i=0;i<4;i++){
        if(prev_mincost[i]!=node2_mincost[i]){
            changed = 1;
        }
    }

    // If distance table changed, send updates to neighbors
    if(changed) {
        struct rtpkt packet;
        packet.sourceid = 2;
        for (i = 0; i < 4; i++)
            packet.mincost[i] = node2_mincost[i];

        printf("\nNode 2 distance table updated to:\n");
        printdt2(&dt2);
        printf("Node 2 sending routing packets to neighbors with mincost: %d %d %d %d\n",
                packet.mincost[0], packet.mincost[1], 
                packet.mincost[2], packet.mincost[3]);
        printf("\n");

        // Send to each neighbor
        packet.destid = 0;
        tolayer2(packet);
        packet.destid = 1;
        tolayer2(packet);
        packet.destid = 3;
        tolayer2(packet);
        
        
    } else {
        printf("Minimum costs NOT updated\n");
        printf("Current distance table for node 2:\n");
        printdt2(&dt2);
    }
}



printdt2(dtptr)
  struct distance_table *dtptr;
  
{
  
  printf("                via     \n");
  printf("   D2 |    0     1    3 \n");
  printf("  ----|-----------------\n");
  printf("     0|  %3d   %3d   %3d\n",dtptr->costs[0][0],
	 dtptr->costs[0][1],dtptr->costs[0][3]);
  printf("dest 1|  %3d   %3d   %3d\n",dtptr->costs[1][0],
	 dtptr->costs[1][1],dtptr->costs[1][3]);
  printf("     3|  %3d   %3d   %3d\n",dtptr->costs[3][0],
	 dtptr->costs[3][1],dtptr->costs[3][3]);
   printf("\n");
}







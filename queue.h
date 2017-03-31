struct node {				
       int pid;
       int value;
	     char *name;
       struct node *next;
	
};


struct queue {				
       struct node *head;
       struct node *tail ;
};


     
void enqueue(int item, char *s, int time, struct queue *q) {		
     struct node *p;						

     p= (struct node *)malloc(sizeof(struct node));
     p->pid=item;
     p->name=s;	
     p->value=time;
     p->next=NULL;
     if (q->head==NULL) q->head=q->tail=p;
     else {
          q->tail->next=p;
          q->tail=p;
     }

}


int dequeue(struct queue *q) {		
    int item;				
    struct node *p;
    
    item=q->head->pid;
    p=q->head;
    q->head=q->head->next;
    free(p);
    return item;
}


void insert(int item, char *s, int time, struct queue *q)
{	//printf("%d",p->pid);
	struct node *temp, *prev, *next;
	temp=malloc(sizeof(struct node));
	temp->pid=item;
   	temp->name=s;	
    	temp->value=time;
    	temp->next=NULL;
	if(q->head == NULL)
	{
		q->head=temp;
	}

	else
	{	next = q->head;
		prev = NULL;
		while(next!=NULL && next->value<=time)
		{
			prev = next;
			next = next->next;
		}
		if(next==NULL)
		{
			prev->next= temp;
		}
		else
		{
			if(prev)
			{
			temp->next=prev->next;
			prev->next=temp;
			}
			else
			{
			temp->next=q->head;
			q->head=temp;
			}
		}
	}
printf("%d\n",q->head->pid);
	
}


void sort(struct queue *q,struct queue *p) {

	while(q->head!=NULL)
	{
	//insert(q->head,p);
	dequeue(q);
	}

}


void delete(struct queue *q, int key)
{
     if (q->head->pid == key)
     {
        struct node *p = q->head;
        q->head = q->head->next;
        free(p);
        return;
    }
    struct node *current = q->head->next;
    struct node *previous = q->head;
    while (current != NULL && previous != NULL)
    {
      if (current->pid == key)
      {
        struct node *tmp = current;
        if (current == q->tail)
          q->tail = previous;
        previous->next = current->next;
        free(tmp);
        return;
      }
      previous = current;
      current = current->next;
    }
    return;  
  }






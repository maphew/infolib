/* strappend.c           A function in misclib.
   ------------------------------------------------------------------------
   int strappend(rec,item,type)
   char *rec, *item, *type;
   
   Appends an item to a string for comma delimited, quoted string
   ascii files.  If type is "C", the item is quoted, otherwise, it's not.
   
   Written: 09/05/95 Randy Deardorff.  USEPA Seattle.
   
   Modification log: (all changes are by the author unless otherwise noted)
   
   ------------------------------------------------------------------------ */

int strappend(rec,item,type)
     char *rec, *item, *type;
{
  if (strcmp(type,"C")==0)
    {
      /* Quoted string. */
      
      if (item[0]=='\0')
        {
          if (rec[0]!='\0')
            strcat(rec,",");
          strcat(rec,"\"\"");
        }
      else
        {
          if (rec[0]!='\0')
            strcat(rec,",");
          strcat(rec,"\"");
          strcat(rec,item);
          strcat(rec,"\"");
        }
    }
  else
    {
      /* Non-quoted string, ie, number. */
      
      if (item[0]=='\0')
        {
          if (rec[0]!='\0')
            strcat(rec,",");
        }
      else
        {
          if (rec[0]!='\0')
            strcat(rec,",");
          strcat(rec,item);
        }
    }
  return 0;
}

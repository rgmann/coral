#ifndef REVISION_H
#define REVISION_H

class Revision : public GenericModel
{
public:
   
   explicit Revision(MongoConnection *pConnection = NULL);

   void file(const File &file);
   bool file(File &user) const;

   /**
    * The owner of a revision is the user that caused the revision.  A revision
    * can only be enacted by one user at a time.
    */
   void owner(const User &owner);
   bool owner(User &user) const;

   /**
    * Group parent of the file at the time of the revision.
    *
    * TBD: Is this really necessary since the file object can provide us
    *      with the group?  All lookups will start at the user, procede to the
    *      group, retrieve the revision ID.  This provides the reference to the
    *      revision document.  To get information about owner, group or user,
    *      access the file document
    */
   void group(const Group &group);
   bool group(Group &group);

   enum Type {
      None
      Created,
      Modified,
      Deleted,
      Copied,
      Merged,
      Moved
   };

   void type(Type type);
   Type type() const;

   Date date() const;

   void summary(const RevisionSummary &summary);
   bool summary(RevisionSummary &summary);

   u64 revision();
};

#endif // REVISION_H
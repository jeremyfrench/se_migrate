Stack Exchange to Drupal migration
=================================

This module will take data from a set of stack exchange dump files and create a Drupal site with that data. The goal is *NOT* to create a fully functional site which can be put online to rival stack exchange. There are two purpouses for this module
 * To act as an example for the migrate module, helping others to learn.
 * (Potentially) to alow people to test their infrastructure at scale with realistic data. As an alternative to devel generate.

The stack exchange people were nice enough to provide this data under a CC licence, and I am re distributing under that licence.  


Current state of module
-----------------------

 * Working for questions and answers with comments but questions and answers are not yet linked
 * Users import except for duplicate user names
 * Tags work for questions

TODO
----

 * voting api for votes on questions answers and comments
 * link questions to answers
 * user profile fields
 * flags for flagged content
 * question history



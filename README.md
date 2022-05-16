# JSON Article Search Engine

## Instructions for Use

- Only one command line argument is required, which is the path of the folder containing the dataset. 
- The console user-interface will begin automatically, and options for engine management and use will be displayed.

#### Query Formatting

- The Boolean expression will be **prefixed** with a Boolean operator of either AND or OR *if there is more than one word of interest*.

- No query will contain both AND and OR.

- Single word queries (not counting NOT or additional operators below) do not need a boolean operator.

- Trailing search terms may be preceded with the NOT operator, which indicates articles containing that term should be removed from the resultset.

- Additional Operators: A query can contain zero or more of the following:
  - ORG <some organization name> - the org operator will search a special index you maintain related to organizations mentioned in the entity metadata
  - PERSON <some person name> - the person operator will search a special index you maintain related to persons mentioned in the article’s entity metadata.
  - Additional Operator Notes:
    - the order of ORG or PERSON doesn’t matter (meaning, you should accept queries that have them in either order)
    - the operators will always be entered in all caps.
- Example Queries:
  - **markets**
    - This query should return all articles that contain the word *markets*.
  - **AND social network**
    - This query should return all articles that contain the words “social” and “network” (doesn’t have to be as a 2-word phrase)
  - **AND social network PERSON cramer**
    - This query should return all articles that contain the words social and network and that mention cramer as a person entity.
  - **AND social network ORG facebook PERSON cramer**
    - This query should return all articles that contain the words social and network, that have an entity organization of facebook and that mention cramer as a person entity.
  - **OR snap facebook**
    - This query should return all articles that contain either snap OR facebook.
  - **OR facebook meta NOT profits**
    - This query should return all articles that contain facebook or meta but that do not contain the word profits.
  - **bankruptcy NOT facebook**
    - This query should return all articles that contain bankruptcy, but not facebook.
  - **OR facebook instagram NOT bankruptcy ORG snap PERSON cramer**
    - This query should return any article that contains the word facebook OR instagram but that does NOT contain the word bankruptcy, and the article should have an organization entity with Snap and a person entity of cramer

## Solution Overview

#### Search Engine

- Our Solution for the Search Engine class implements heavy use of the Processor class and the QueryBuilder class. The Processor class utilizes multithreading to enhance speed of file parsing and contains all methods that relate to manipulating the article index, this includes building the article index, manipulating the AVL-Tree indexes, and printing article information. The QueryBuilder class contains all methods that relate to handling the queries entered by the current user, this includes handling query logic and searching the AVL-Tree indexes.
- The QueryBuilder class utilizes a Divide & Conquer algorithm to handle all query logic. The QueryBuilder class was designed with a tree based structure, which is used to handle all sub-queries that were created by breaking up the original query. Each major portion of the original-query is placed into a different leaf node and processed as their own query. Once all sub-queries are processed, all separate leaf nodes get bubbled up and combined back into one fully processed query.
- This Search Engine solution uses a console driven user-interface that was made to be as ergonomic and visually intuitive as possible, all by using colors and different console-based visual manipulations.
- All features provided by the Search Engine User-Interface:
  - Populating engine data by parsing JSON documents
  - Populating engine data from caches
  - Deleting a word from the search engine
  - Managing the AVL tree cache
  - Managing the article cache
  - Entering a query
  - Viewing the results of a query
  - Viewing an article from the list of query results
  - viewing the search engine statistics
  - Clearing the engine data
  - Ending the program

#### AVL-Tree

- This Search Engine solution indexes all provided articles into a custom AVL-Tree designed by this team.
- Insertion into this AVL-Tree is iterative and O(lg n) time. Balance is maintained for all insertions.
- Deletion of nodes from this AVL-Tree is O(lg n) time. Balance is maintained for all deletions.
- You can serialize this AVL-Tree into an archive file of choice, this archiving algorithm uses a JSON formatting that is driven by the [Cereal Serialization Library](https://uscilab.github.io/cereal/). This Library also supports deserialization from an archive file, so that feature is supported and present for the AVL-Tree as well.
- Other features implemented for this AVL-Tree solution:
  - In-order key printing
  - Level-order key printing
  - Level-order node serialization
  - Node deserialization
  - Different insertion methods that act different for the case where the passed key is already present within the tree: The past value is overwritten, the new value is appended to the past value, or a custom appending method is provided by the user.

#### Hash-Table

- This Search Engine solution uses a custom hash-table designed by the team to track all indexed organizations and people.
- The maximum load factor that this Hash-Table is allowed to have is 75%, when this load factor is reached, the hash-table doubles its size.
- When there are collisions, the indexing keys algorithm within the Hash-Table utilizes linear traversal to search for the next available bucket.
- Other features implemented for this Hash-Table solution:
  - Iterators
  - Support for Custom hash functions
  - Merging Hash-Tables
  - Masking Hash-Tables (Overwriting the elements of a Hash-Table that overlap with another the elements of another Hash-Table)

## Helpful Links

- [Data Set Download Page](https://www.kaggle.com/datasets/jeet2016/us-financial-news-articles)
- [C++ Porter 2 Stemmer Library](https://github.com/smassung/porter2_stemmer)
- [Cereal Serialization Library](https://uscilab.github.io/cereal/)

## Credits

- Drew Harris
  - [GitHub](https://github.com/drew-harris)
- Adam Escobedo
  - [LinkedIn](https://www.linkedin.com/in/adamescobedo/)
  - [GitHub](https://github.com/adamcesco)

# JSON Article Search Engine

## Instructions for Use

- Only one command line argument is required, and it needs to be the path to the folder containing the dataset. 
- The console user-interface will begin automatically, and options for engine management and use will be displayed.

#### Query Formatting

- The Boolean query will be **prefixed** with a Boolean operator of either AND or OR (*if there is more than one word of interest*).
- No query will contain both AND and OR.
- Single word queries do not need a boolean operator.
- Trailing search terms may be preceded with the NOT operator, which indicates articles containing that term should be removed from the results.
- Additional Operators: A query can contain zero or more of the following:
  - ORG <some organization name> - the org operator narrows the results of a query to only include articles that contain the mentioned organization. Replace spaces within an organization name with dashes (-).
  - PERSON <some person name> - the PERSON operator narrows the results of a query to only include articles that contain the mentioned person. People are searched for on a first-name/last-name basis, place a dash (-) between the two.
  - Additional Operator Notes:
    - the order of ORG or PERSON doesn’t matter
    - the operators should always be entered in all caps.
- Example Queries:
  - **market**
    - This query should return all articles that contain the word "market".
  - **AND social network**
    - This query should return all articles that contain the words “social” and “network”.
  - **AND social network PERSON mark-zuckerberg**
    - This query should return all articles that mention Mark Zuckerberg as a person entity and also contain the words social and network.
  - **AND social network PERSON mark-zuckerberg ORG facebook**
    - This query should return all articles that contain the words social and network, that have an entity organization of facebook, and that mention Mark Zuckerberg as a person entity.
  - **OR snapchat facebook**
    - This query should return all articles that contain either snapchat OR facebook.
  - **OR facebook meta NOT profits**
    - This query should return all articles that contain facebook or meta but do not contain the word profits.
  - **phone NOT AT&T**
    - This query should return all articles that contain the word phone, but not AT&T.
  - **OR facebook instagram NOT bankruptcy ORG snapchat PERSON mark-zuckerberg**
    - This query should return any article that contains the word facebook OR instagram but does NOT contain the word bankruptcy, and each article within the results should contain the organization Snapchat and the person Mark Zuckerberg.
- Additional Query Information: Stop-words are not indexed within the Search Engine, so all stop-words present within a query will not be considered nor affect the results.

## Solution Overview

#### Search Engine

- Our Solution for the Search Engine class implements heavy use of the Processor class and the QueryBuilder class. The Processor class utilizes multithreading to enhance speed of file parsing and contains all methods that relate to manipulating the article index, this includes building the article index, manipulating the AVL-Tree indexes, and printing article information. The QueryBuilder class contains all methods that relate to handling the queries entered by the current user, this includes handling query logic and searching the AVL-Tree indexes.
- The QueryBuilder class utilizes a Divide & Conquer algorithm to handle all query logic. The QueryBuilder class was designed with a tree based structure, which is used to handle all sub-queries that were created by breaking up the original query. Each major portion of the original-query is placed into a different leaf node and processed as their own query. Once all sub-queries are processed, all separate leaf nodes get bubbled up and combined back into one fully processed query.
- This Search Engine solution uses a console driven user-interface that was made to be as ergonomic and visually intuitive as possible, all by using colors and different console-based visual manipulations.
- All features provided by the Search Engine User-Interface:
  - Populate engine data by parsing the provided data-set
  - Populate engine data from the present caches
  - Delete a word from the search engine
  - Manage the AVL tree cache
  - Manage the article cache
  - Enter a query
  - View the top-15 results of a query
  - View an article from the list of query results
  - View the search engine statistics
  - Clear all engine data
  - End the program

#### AVL-Tree

- This Search Engine solution indexes all provided articles into a custom AVL-Tree designed by this team.
- Insertion into this AVL-Tree is iterative and O(lg n) time, where n is the number of nodes within the AVL-Tree. Balance is maintained for all insertions.
- Deletion of nodes from this AVL-Tree is O(lg n) time, where n is the number of nodes within the AVL-Tree. Balance is maintained for all deletions.
- You can serialize this AVL-Tree into an archive file of choice, this archiving algorithm uses JSON formatting that is driven by the [Cereal Serialization Library](https://uscilab.github.io/cereal/). This Library also supports deserialization from an archive file of choice, so that feature is supported and present for the AVL-Tree as well.
- Other features implemented for this AVL-Tree solution:
  - In-order key printing
  - Level-order key printing
  - Level-order node serialization
  - Node deserialization
  - Different insertion methods that act differently in the scenario where the given key is already associated with a value within the tree: Depending on the used insertion method, the previous value associated with the key is overwritten, the new value is appended to the previous value, or a custom overwriting/appending method is provided by the user.

#### Hash-Table

- This Search Engine solution uses a custom hash-table designed by the team to track all indexed organizations and people.
- The maximum load factor that this Hash-Table is allowed to have is 75%, when this load factor is reached, the hash-table doubles its size.
- When there are collisions, the indexing keys algorithm within the Hash-Table utilizes linear traversal to search for the next available bucket.
- Other features implemented for this Hash-Table solution:
  - Iterators
  - Support for Custom hash functions
  - Merging Hash-Tables
  - Masking Hash-Tables

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

<?php

/*
 * You must implement hook_migrate_api(), setting the API level to 2, for
 * your migration classes to be recognized by the Migrate module.
 */
function se_migrate_migrate_api() {
  $api = array(
    'api' => 2,
     'migrations' => array(
       'MigrateSeUser' => array('class_name' => 'MigrateSeUser'),
       'MigrateSeQuestions' => array('class_name' => 'MigrateSeQuestions'),
       'MigrateSeAnswers' => array('class_name' => 'MigrateSeAnswers'),
       'MigrateSeComments' => array('class_name' => 'MigrateSeComments'),
     )
  );
  return $api;
}

class MigrateSeUser extends XMLMigration {
  /* construct method */  
  public function __construct() {
    parent::__construct(MigrateGroup::getInstance('se_migrate'));
    
    // Source definition
    $path = drupal_get_path('module','se_migrate') . '/data/users.xml';
    $path_url = url($path,array('absolute' => TRUE));
    // Xpath query to get items
    $query = '/users/row';
    // Xpath query withn the item query to get a unique id
    $id_query = '@Id';
    
    /**
     * Sample line of the xml, indented for clarity
     *   <row Id="182"
     *        Reputation="4557"
     *        CreationDate="2011-03-03T07:59:42.010"
     *        DisplayName="Jeremy French"
     *        EmailHash="15ddf4d0e44ae1d9181a49a3aa836f86"
     *        LastAccessDate="2012-07-26T08:57:45.133"
     *        WebsiteUrl="http://www.jeremyfrench.co.uk"
     *        Location="United Kingdom"
     *        Age="36" 
     *        AboutMe="<SNIP>"
     *        Views="0"
     *        UpVotes="517"
     *        DownVotes="33" />
     */
    
    // Fields, need to be defined for XML types
    $fields = array(
      'reputation' => t('Reputation'),
      'username' => t('Username'),
      'created' => t('Registration date'),
      'access' => t('Last Access'),
      'webiste_url' => t('Website Url'),
      'location' => t('Geographic location'),
      'age' => t('Age (as of Sep 2011)'),
      'up_votes' => t('Up votes'),
      'down_votes' => t('Down votes'),
    );
    
    $this->source = new MigrateSourceXML($path_url, $query, $id_query, $fields);
    
    // Destination definition
    $this->destination = new MigrateDestinationUser();
    

    
    // We instantiate the MigrateMap
    $this->map = new MigrateSQLMap($this->machineName,
        array(
          'sourceid' => array('type' => 'int',
                          'unsigned' => FALSE,
                          'not null' => TRUE,
                         )
        ),
        MigrateDestinationUser::getKeySchema()
      );
      
      
    // Add mappings
    $this->addFieldMapping('name', 'username')
         ->xpath('@DisplayName');
    
    $this->addFieldMapping('created', 'created')
         ->xpath('@CreationDate');
         
    
    /* $this->addFieldMapping('field_website','webiste_url')
         ->xpath('@WebsiteUrl');*/
         
    // Some Fields can have a default without a mapping, will be the same for all records.
    $this->addFieldMapping('pass')->defaultValue('password');
    $this->addFieldMapping('mail')->defaultValue('test@example.com');
    $this->addFieldMapping('status')->defaultValue(1);
         
    
    $this->description = t('Migrate Stack Exchange users to Drupal.'); 
  }
  
}


/**
 * Questions and answers are rather similar so define a base class that they can each extend. 
 */

abstract class MigrateSeContent extends XMLMigration {
   public function __construct() {
    parent::__construct(MigrateGroup::getInstance('se_migrate'));
    
    $this->dependencies = array('MigrateSeUser');
    
    // Source definition
    $path = drupal_get_path('module','se_migrate') . '/data/posts.xml';
    $this->path_url = url($path,array('absolute' => TRUE));
    
   
    // Xpath query withn the item query to get a unique id
    $this->id_query = '@Id';
    
    $this->fields = array(
      'CreationDate' => t('creation date'),
      'Score' => t('score'),
      'ViewCount' => t('view count'),
      'Body' => t('body'),
      'OwnerUserId' => t('owner id'),
      'LastEditorUserId' => t('last editor id'),
      'LastEditorDisplayName' => t('last editor name'),
      'LastEditDate' => t('last edit date'),
      'LastActivityDate' => t('Last activity'),
      'Title' => t('title'),
      'Tags' => t('tags'),
      'CommentCount' => t('Comment count'),
    );
    
    
    $this->map = new MigrateSQLMap($this->machineName,
        array(
          'sourceid' => array('type' => 'int',
                          'unsigned' => FALSE,
                          'not null' => TRUE,
                         )
        ),
        MigrateDestinationUser::getKeySchema()
      );

    $this->addFieldMapping('body', 'Body')
         ->xpath('@Body');
         
    $this->addFieldMapping('created', 'CreationDate')
         ->xpath('@CreationDate');
         
    $this->addFieldMapping('changed', 'LastEditDate')
         ->xpath('@LastEditDate');
         
    
    $this->addFieldMapping('uid','OwnerUserId')
         ->xpath('@OwnerUserId')
         ->sourceMigration('MigrateSeUser')
         ->defaultValue(1);     
    
    $this->addFieldMapping('revision_uid','LastEditorUserId')
         ->xpath('@OwnerUserId')
         ->sourceMigration('MigrateSeUser');
    
    
    // Static values
    $this->addFieldMapping('language')->defaultValue('en');
    $this->addFieldMapping('body:format')->defaultValue('filtered_html');
    
    
    
    // Add migration meta data
    $this->description = t('Migrate Stack Exchange users to Drupal.'); 
    $this->team = array(new MigrateTeamMember('Jeremy French','jeremy@jeremyfrench.co.uk','admin'));
    
    }  
}

class MigrateSeQuestions extends MigrateSeContent  {
  /** Post format 
   *  <row Id="1"
   *       PostTypeId="1"
   *       AcceptedAnswerId="393"
   *       CreationDate="2011-03-02T20:49:08.213"
   *       Score="9"
   *       ViewCount="2140"
   *       Body="&lt;p&gt;How can I change the default Apache Solr URL path from &quot;search/apachesolr_search/term&quot; to something else?&lt;/p&gt;&#xA;"
   *       OwnerUserId="14"
   *       LastEditorUserId="199"
   *       LastEditorDisplayName=""
   *       LastEditDate="2011-08-02T16:50:51.567"
   *       LastActivityDate="2011-11-09T17:11:18.020"
   *       Title="How can I change the Apache Solr search URL in Drupal?"
   *       Tags="&lt;7&gt;&lt;search&gt;&lt;apache-solr&gt;"
   *       AnswerCount="8"
   *       CommentCount="1"
   *       FavoriteCount="4" />
   */
   /* construct method */  
  public function __construct() {
    parent::__construct(MigrateGroup::getInstance('se_migrate'));
    // Extend items not set up in the base class.
    $this->description = t('Migrate stack exchange questions into se_question content type');
        
     // Xpath query to get items PostTypeId = 1 is questions.
    $query = '/posts/row[@PostTypeId="1"]';
    
    // Add fields which are specific to questions.
    $fields = $this->fields;
    $fields['Title'] = t('title');
    $fields['Tags'] = t('tags');
    $fields['AnswerCount'] = t('Answer Count');
    $fields['FavoriteCount'] = t('Fav count');
    
    
    // Sorce needs to be defiend with complete fields
    $this->source = new MigrateSourceXML($this->path_url, $query, $this->id_query, $fields);
    
    // Destination definition
    $this->destination = new MigrateDestinationNode('sequestion');
    
    // Add mappings for distinct fields.
    $this->addFieldMapping('title', 'Title')
         ->xpath('@Title');
   
    /* Tags */
    $this->addFieldMapping('field_tags', 'Tags')
         ->xpath('@Tags')
         ->separator(',');
         

    $this->addFieldMapping('field_tags:create_term')->defaultValue(TRUE);
    $this->addFieldMapping('field_tags:ignore_case')->defaultValue(TRUE);
    
    // TODO: add view count.
        
  }

  /**
   * Prepare row gives a chance to alter the data in a source row before
   * it is passed into the source.
   */
  function prepareRow($row) {
    //Turn XML list into comma seperated list  
    $attributes = $row->xml->attributes();
    $tags = $attributes['Tags'];
    $tags = str_replace('>',',',$tags);
    $tags = str_replace('<', '',$tags);
    $row->xml['Tags'] = $tags;    
    
  }
    
}

class MigrateSeAnswers extends MigrateSeContent  {
  public function __construct() {
    parent::__construct(MigrateGroup::getInstance('se_migrate'));
    // Extend items not set up in the base class.
    $this->description = t('Migrate stack exchange answers into se_answer content type');
     // Xpath query to get items PostTypeId = 1 is questions.
    $query = '/posts/row[@PostTypeId="2"]';
    
    
    // Sorce needs to be defiend with complete fields
    $this->source = new MigrateSourceXML($this->path_url, $query, $this->id_query, $this->fields);
    
    // Destination definition
    $this->destination = new MigrateDestinationNode('seanswer');
    } 
}


// Migration for comments
class MigrateSeComments extends XMLMigration {
   public function __construct() {
    parent::__construct(MigrateGroup::getInstance('se_migrate'));
    $this->description = array('Migrate comments for se_question and se_answer content types');
    $this->dependencies = array('MigrateSeAnswers','MigrateSeQuestions');

    // Source definition
    $path = drupal_get_path('module','se_migrate') . '/data/comments.xml';
    $path_url = url($path,array('absolute' => TRUE));
    
    $query = '/comments/row';
   
    // Xpath query withn the item query to get a unique id
    $id_query = '@Id';
    
    $fields = array(
      'PostId' => t('Post Id'),
      'Score' => t('score'),
      'Text' => t('comment text'),
      'CreationDate' => t('creation date'),
      'UserId' => t('User Id'),
    );
    
    $this->source = new MigrateSourceXML($path_url, $query, $id_query, $fields);
      
    // Destination definition
    $this->destination = new MigrateDestinationComment('comment_node_sequestion');
    /**
     * Note that above is a little bit of a hack. sequestion and seanswer node 
     * comments are the same format and the destination only looks at the structure
     * so this migrates ok. 
     */
    
    $this->map = new MigrateSQLMap($this->machineName,
        array(
          'sourceid' => array('type' => 'int',
                          'unsigned' => FALSE,
                          'not null' => TRUE,
                         )
        ),
        MigrateDestinationComment::getKeySchema()
      );
    
     $this->addFieldMapping('nid', 'PostId')
          ->sourceMigration(array('MigrateSeQuestions','MigrateSeAnswers'))     
          ->xpath('@PostId');
         
     $this->addFieldMapping('uid','UserId')
         ->xpath('@UserId')
         ->sourceMigration('MigrateSeUser')
         ->defaultValue(1);     
     
     $this->addFieldMapping('comment_body','Text')
         ->xpath('@Text');
     
     $this->addFieldMapping('created','CreationDate')
         ->xpath('@CreationDate');

     $this->addFieldMapping('status')->defaultValue(TRUE);  
     $this->addFieldMapping('language')->defaultValue('en');  
        
   }
}
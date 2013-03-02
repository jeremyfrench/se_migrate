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
         
    
    $this->addFieldMapping('field_website','webiste_url')
         ->xpath('@WebsiteUrl');
         
    // Some Fields can have a default without a mapping, will be the same for all records.
    $this->addFieldMapping('pass')->defaultValue('password');
    $this->addFieldMapping('mail')->defaultValue('test@example.com');
    $this->addFieldMapping('status')->defaultValue(1);
         
    
    $this->name = t('Migrate Stack Exchange user');
    $this->description = t('Migrate Stack Exchange users to Drupal.'); 
    
  }
  
}


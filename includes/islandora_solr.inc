<?php

  /**
   * @file Functionality for the islandora_solr Module
   * @author griffinj@lafayette.edu
   *
   */

  /**
   * Implements hook_preprocess_theme()
   * @see islandora_solr_islandora_solr
   *
   */
function bootstrap_dss_digital_preprocess_islandora_solr(&$variables) {

  $display = (empty($_GET['display'])) ? 'list' : $_GET['display'];

  $path = current_path();

  $query_params['display'] = 'list';
  $list_link = array(
    'title' => t('List view'),
    'attributes' => array(
      'href' => url($path, array('query' => $query_params)),
      'class' => array('islandora-view-list'),
    ),
  );

  $query_params['display'] = 'grid';
  $grid_link = array(
    'title' => t('Grid view'),
    'attributes' => array(
      'href' => url($path, array('query' => $query_params)),
      'class' => array('islandora-view-grid'),
    ),
  );

  $variables['display'] = $display;

  /**
   * Work-around for resolving issues related to MARC terms
   * Provides a map for overriding certain Solr field values
   * @todo Refactor and integrate variable_get()
   *
   * This resolves DSS-167
   * This resolves DSS-170
   */
  $ISLANDORA_MARC_METADB_MAP = array(
				     'eastasia.Format.Medium' => array(
								       "photonegative" => 'Photographic negative',
								       "photoprint" => 'Photographic print',
								       "slide" => 'Photographic slide',
								       "picture postcard" => 'Picture postcard',
								       ),
				     'mdl_prints.format.medium' => array(
									 "photoprint" => 'lithograph',
									 )
				     );

  if(preg_match('/cdm\.Relation\.IsPartOf\:"(.+?)"/', current_path(), $m)) {

    $relation = $m[1];

    $relation_is_part_of_dc_field_map = array(
					      'Marquis de Lafayette Prints Collection' => array(
												'dc.description',
												'dc.format',
												'dc.identifier',
												'dc.rights',
												'dc.subject',
												'dc.type'
												),
					      'John S. Shelton Earth Science Image Collection' => array('dc.contributor',
													'dc.coverage',
													'dc.description',
													'dc.format',
													'dc.identifier',
													'dc.language',
													'dc.publisher',
													'dc.subject',
													'dc.type',
													)
					      );

    foreach($variables['results'] as &$result) {

      foreach($result['solr_doc'] as $field_name => &$field) {


	if(in_array($relation, $relation_is_part_of_dc_field_map)) {

	    if(in_array($field_name, $relation_is_part_of_dc_field_map[$relation])) {

	      unset($result['solr_doc'][$field_name]);
	    }

	}

      }
    }
  }

  foreach($variables['results'] as &$result) {

    foreach($result['solr_doc'] as $field_name => &$field) {

      /**
       * Work-around for DSS-167
       * Work-around for DSS-170
       *
       */
      if(array_key_exists($field_name, $ISLANDORA_MARC_METADB_MAP)) {

	if(array_key_exists($field['value'], $ISLANDORA_MARC_METADB_MAP[$field_name])) {

	  $field['value'] = $ISLANDORA_MARC_METADB_MAP[$field_name][$field['value']];
	}
      }
    }
  }

  // For rendering non-grid content
  drupal_add_css(drupal_get_path('module', 'islandora_basic_collection') . '/css/islandora_basic_collection.base.css');
  drupal_add_css(drupal_get_path('module', 'islandora_basic_collection') . '/css/islandora_basic_collection.theme.css');
}

function bootstrap_dss_digital_islandora_discovery_form($form, &$form_state, $collection_fields, $sorted_field) {

  $form = array('#attributes' => array('id' =>'islandora-discovery-form'));

  if(!array_key_exists('sort', $_GET) or stripos($_GET['sort'], 'asc')) {

    $asc_anchor_classes = array('field-sort', 'active');
    $desc_anchor_classes = array('field-sort');
  } else {

    $asc_anchor_classes = array('field-sort');
    $desc_anchor_classes = array('field-sort', 'active');
  }

  $prefix = '<div class="islandora-discovery-control title-sort-control"><span>Sort by:</span>';
  $prefix .= l('Asc', NULL, array('attributes' => array('class' => $asc_anchor_classes,
							   'id' => 'field-sort-asc'),
				     'external' => TRUE,
				     'fragment' => FALSE));
  $prefix .= l('Desc', NULL, array('attributes' => array('class' => $desc_anchor_classes,
							   'id' => 'field-sort-desc'),
				     'external' => TRUE,
				     'fragment' => FALSE));
  $suffix = '</div>';

  $form['field_sort_select'] = array('#type' => 'select',
				     '#options' => $collection_fields,
				     '#default_value' => $sorted_field,
				     //'#default_value' => 'dc.date.sort',
				     '#attributes' => array('id' => 'field-sort-select'),
				     '#prefix' => $prefix,
				     '#suffix' => $suffix,
				     );

  return $form;
}

function bootstrap_dss_digital_process_islandora_solr_wrapper(&$variables) {

  $display = (empty($_GET['display'])) ? variable_get('islandora_basic_collection_default_view', 'grid') : $_GET['display'];
  $variables['display'] = $display;

  $path = current_path();

  $query_params['display'] = 'list';
  $list_link = array(
    'title' => t('List'),
    'attributes' => array(
      'href' => url($path, array('query' => $query_params)),
      'class' => array('islandora-view-list'),
    ),
  );

  $query_params['display'] = 'grid';
  $grid_link = array(
		     'title' => t('Grid'),
		     'attributes' => array('href' => url($path, array('query' => $query_params)),
					   'class' => array('islandora-view-grid'),
					   ),
		     );

  $refine_link = array(
		       'title' => t('Refine'),
		       'attributes' => array('href' => '#',
					     'class' => array('snap-trigger'),
					     'style' => ''));

  // Refactor
  $view_icon_srcs = array('Refine' => base_path() . drupal_get_path('theme', 'bootstrap_dss_digital') . '/files/RefineIcon.png',
			  'List' => base_path() . drupal_get_path('theme', 'bootstrap_dss_digital') . '/files/ListIcon.png',
			  'Grid' => base_path() . drupal_get_path('theme', 'bootstrap_dss_digital') . '/files/GridIcon.png');

  $variables['view_icon_srcs'] = $view_icon_srcs;

  // Refactor
  $view_icon_alts = array('Refine' => 'refine search icon',
			  'List' => 'view list icon',
			  'Grid' => 'view grid icon');

  $variables['view_icon_alts'] = $view_icon_alts;

  // Refactor
  $view_icon_ids = array('Refine' => 'refine-search-icon',
			 'List' => 'view-list-icon',
			 'Grid' => 'view-grid-icon');

  $variables['view_icon_ids'] = $view_icon_ids;

  $variables['view_links'] = array($refine_link, $list_link, $grid_link);


  $elements = $variables['elements'];
  drupal_set_title($elements['solr_total'] . ' Items Found');

  $collection_fields = $variables['collection_fields'];

  /**
   * Ensures that searching within and browsing all repository content only permits sorting by the dc.title and dc.contributor fields
   * Resolves DSSSM-789
   *
   */
  if(!array_key_exists('f', $_GET)) {

    $collection_fields = array('dc.title' => 'Title',
			       //'dc.publisher.sort' => 'Author',
			       );
  }

  if(array_key_exists('sort', $_GET) and preg_match('/(.+?) /', $_GET['sort'], $m)) {

    $sorted_field = $m[1];
  } else {

    /**
     * If this is a member of the newspaper collection, sort by the publication date
     * Resolves DSSSM-865
     *
     */
    $sorted_field = 'dc.title';
    $params = $_GET;

    if(array_key_exists('f', $params) and !empty($params['f'])) {

      if($params['f'][0] == 'cdm.Relation.IsPartOf:"Lafayette Newspaper Collection"') {

        $sorted_field = 'dc.date.sort';
      }
    }
  }

  /**
   * Work-around for disabling sorting for global repository browsing and searches
   * @todo Render once Solr has indexed the appropriate fields for sorting
   * Resolves DSSSM-793
   *
   */
  if(!array_key_exists('f', $_GET)) {

    $variables['islandora_discovery_form'] = '';
  } else {

    $variables['islandora_discovery_form'] = drupal_render_children(drupal_get_form('bootstrap_dss_digital_islandora_discovery_form', $collection_fields, $sorted_field));
  }
}

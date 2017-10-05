<?php

function flag_app_form($form, $form_state) {
  $form = array();
  
  $header = array(
    'title'=> t('Flagged Content'),
    'name'=> t('Username'),
    'uid'=> t('User ID'),
    'entity_id'=> t('Node ID'),
    'flagging_id'=> t('Flag ID'),
    'status'=> t('Status'),
  );
  
  $query = db_select('flag_app', 'fa')->extend('PagerDefault');
  $query->join('flagging', 'f', 'f.flagging_id = fa.flagging_id');
  $query->join('node', 'n', 'n.nid = f.entity_id');
  $query->join('users', 'u', 'f.uid = u.uid');
  $query
        ->fields('fa', array('status'))
        ->fields('f', array('flagging_id', 'entity_id', 'timestamp'))
        ->fields('n', array('title'))
        ->fields('u', array('uid', 'name'))
        ->orderBy('timestamp', 'DESC')
        ->limit(25);
  $results = $query->execute();
  
  $rows = array();

  foreach($results as $result){
    switch($result->status){
      case 0:
        $status = "Pending";
        break;
      case 1:
        $status = "Accepted";
        break;
      case 2:
        $status = "Rejected";
        break;
    }
    
    $rows[] = array(
       'title'=> l($result->title, 'node/'.$result->entity_id),
       'name'=> l($result->name, 'user/'.$result->uid),
       'uid'=> $result->uid,
       'entity_id'=> $result->entity_id,
       'flagging_id'=> $result->flagging_id,
       'status'=> $status,
       '#attributes'=> array('class'=> array('application-row')),
    );
  }
  
  $form['flag_app_status']['AcceptReject']= array(
    '#type'=> 'select',
    '#title'=> 'Action',
    '#options'=> array(
      1 => t('Accept'),
      2 => t('Reject'),
    ),
  );
  
  $form['flag_app_table'] = array(
    '#type'=> 'tableselect',
    '#header'=> $header,
    '#options'=> $rows,
    '#empty'=> t('no application'),
    '#attributes'=> array('class' => array('applications')),
  );
  
  $form['pager'] = array('#markup' => theme('pager'));
  
  $form['submit'] = array(
    '#type' => 'submit',
    '#value' => t('Save'),
  );
  
  return $form;
}


function flag_app_form_submit($form, $form_state){
  //dsm($form_state);
  $action = $form_state['values']['AcceptReject'];
  $values = array();
  foreach($form_state['values']['flag_app_table'] as $key => $value){
    if(is_string($value) == TRUE){
      $values[] = $form_state['complete form']['flag_app_table']['#options'][$value];
    }
  }
  flag_app_admin($action, $values);
  
}

function flag_app_form_validate($form, $form_state){
  if($form_state['values']['AcceptReject'] != 1 && $form_state['values']['AcceptReject'] != 2){
    form_set_error('AcceptReject', "Incorrect value");
  }
}

function flag_app_admin($action, $values){
    $status = ($action == 1) ? 'Accepted' : 'Rejected';
    foreach ($values as $key => $value) {
      $query = db_update('flag_app')
          ->fields(array('status' => $action))
          ->condition('flagging_id', $value['flagging_id'])
          ->execute();
    }
}
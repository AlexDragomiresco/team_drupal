<?php

function flag_app_form($form, $form_state) {
  $form = array();
  
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
  $result = $query->execute();
  return $form;
}

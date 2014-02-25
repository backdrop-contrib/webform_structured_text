<?php
/**
 * Include file for structured text component.
 **/

/**
 * Implements _webform_defaults_component().
 */
function _webform_defaults_structured_text() {
  return array(
    'name' => '',
    'form_key' => NULL,
    'mandatory' => 0,
    'pid' => 0,
    'weight' => 0,
    'extra' => array(
      'mask' => '',
      'auto_skip' => TRUE,
      'mask_description' => FALSE,
      'mask_regex' => '',
      'description' => '',
      'analysis_display' => FALSE,
      'analysis_display_identifying' => '',
    ),
  );
}

/**
 * Implements _webform_theme_component().
 */
function _webform_theme_structured_text() {
  return array(
    'webform_structured_text' => array(
      'arguments' => array('element' => NULL),
      'file' => 'structured_text.inc',
    ),
  );
}

/**
 * Implements _webform_edit_component().
 */
function _webform_edit_structured_text($component) {
  $form = array();

  $form['extra']['mask'] = array(
    '#type' => 'textfield',
    '#title' => t('Input mask'),
    '#default_value' => $component['extra']['mask'],
    '#description' => t('Enter an input mask.  Use the following special characters to represent
          parts of the mask.<br><br>
          9 - only a digit can be input<br>
          x - any character can be input<br>
          a - only letters can be input<br>
          ^9 - outputs the character "9"<br>
          ^x - outputs the character "x"<br>
          ^a - outputs the character "a"<br>
          ^^ - outputs the character "^"<br><br>
          all other characters in the mask will be rendered as mark-up around / between
          the textboxes for the portions of the mask that are denoted by the digit nine,
          or lower-case characters "x" or "a".  If you want a certain combination of
          letters and numbers, use "x" as your mask character, and specify a RegEx
          pattern(s) below to check the pattern.'),
    '#weight' => -3.4,
    '#required' => TRUE,
  );
  $form['extra']['auto_skip'] = array(
    '#type' => 'checkbox',
    '#title' => t('Auto-skip to next chunk'),
    '#default_value' => $component['extra']['auto_skip'],
    '#description' => t('Individual chunks of the field are rendered as text boxes
          with markup in between for parts of the mask that are not the digit nine,
          or lower-case characters "x" or "a".  Check this box to have the cursor
          automatically move to the next chunk in the field when the previous one
          is filled.' ),
    '#weight' => -3.3,
  );
  $form['extra']['mask_description'] = array(
    '#type' => 'checkbox',
    '#title' => t('Include mask description'),
    '#default_value' => $component['extra']['mask_description'],
    '#description' => t('Check this box to show a description of the mask in the field\'s
          description area.  This will appear appended to the description you specify
          below (if any).  If you don\'t profide a description below and this box
          is checked, the mask description will still show.' ),
    '#weight' => -3.2,
  );
  $form['extra']['mask_regex'] = array(
    '#type' => 'textarea',
    '#title' => t('Mask RegEx validation'),
    '#default_value' => $component['extra']['mask_regex'],
    '#description' => t('If you want to further validate the pattern of a chunk, you
          can do that by specifying a RegEx expression here that will be used to
          further validate the user\'s input.  Each validation should be on a separate
          line, in the form<br><br>
          <em>chunk-number</em>|<em>error-message</em>|<em>regex-expression</em><br><br>
          with each portion separated by a vertical bar / pipe character, where:<br>
          - <em>chunk-number</em> is an integer corresponding to the ordinal position
          of the chunk,<br>
          - <em>error-message</em> is the error message to display if the user input
          for that chunk doesn\'t match the RegEx expression, and<br>
          - <em>regex-expression</em> is the expression to match against.<br><br>
          For example, to validate a Canadian postal code, which has the format
          letter-number-letter number-letter-number (eg., M5B 2J7), if you wanted
          two chunks, the mask would be "xxx xxx", with validations being<br><br>
          1|First part of postal code must be letter-number-letter.|/[A-Z][0-9][A-Z]/<br>
          2|Second part of postal code must be number-letter-number.|/[0-9][A-Z][0-9]/<br><br>
          You do not have to specify a validation for every chunk.  For more on
          RegEx expressions and preg_match (which is what is used), see !link.',
          array('!link' => '<a href="http://ca2.php.net/preg_match" target=_blank>http://ca2.php.net/preg_match</a>',)),
    '#weight' => -3.1,
  );
  $form['extra']['analysis_display'] = array(
    '#type' => 'checkbox',
    '#title' => t('Show responses in analysis'),
    '#default_value' => $component['extra']['analysis_display'],
    '#description' => t('Show each answer in the analysis results'),
    '#weight' => 3,
    '#parents' => array('extra', 'analysis_display'),
  );
  $other_components = array('' => t('Submission ID'));
  $result = db_query('SELECT cid, form_key FROM {webform_component} WHERE nid = %d ORDER BY weight, form_key', $component['nid']);
  while ( $c = db_fetch_object($result) ) {
    $other_components[$c->cid] = $c->form_key;
  }
  $form['extra']['analysis_display_identifying'] = array(
    '#type' => 'select',
    '#title' => t('Identifying field'),
    '#default_value' => $component['extra']['analysis_display_identifying'],
    '#description' => t('Which component do you want to use to identify results?'),
    '#options' => $other_components,
    '#weight' => 4,
    '#parents' => array('extra', 'analysis_display_identifying'),
  );
  return $form;
}

/**
 * Implements _webform_render_component().
 */
function _webform_render_structured_text($component, $value = NULL, $filter = FALSE) {
  $description = webform_structured_text_field_description($component);
  $form_item = array(
    '#type' => 'webform_structured_text',
    '#input' => TRUE,
    '#title' => $filter ? _webform_filter_xss($component['name']) : $component['name'],
    '#title_display' => $component['extra']['title_display'] ? $component['extra']['title_display'] : 'before',
    '#weight' => $component['weight'],
    '#required' => $component['mandatory'],
    '#description'   => $filter ? _webform_filter_descriptions($description) : $description,
    '#process' => array('webform_structured_text_expand_field',),
    '#pre_render' => array('webform_element_title_display'),
    '#post_render' => array('webform_element_wrapper'),
    '#prefix' => '<div class="webform-component-' . $component['type'] . '" id="webform-component-' . $component['form_key'] . '">',
    '#suffix' => '</div>',
    '#theme' => 'webform_structured_text',
    '#theme_wrappers' => array('webform_element_wrapper'),
    '#element_validate' => array('webform_validate_structured_text'),
    '#mask' => webform_structured_text_parse_mask($component['extra']['mask']),
    '#mask_regex' => $component['extra']['mask_regex'],
    '#auto_skip' => $component['extra']['auto_skip'],
  );

  if (isset($value)) {
    $form_item['#default_value'] = webform_structured_text_parse_value($form_item['#mask'], $value[0]);
  }

  return $form_item;
}

/**
 * Form API #process function for Webform structured text fields.  Breaks up the
 * form element into the combination of mark-up and text input fields specified
 * in the mask.
 */
function webform_structured_text_expand_field($element) {
  static $js_added = FALSE;
  $js_ids = array();
  foreach ( $element['#mask'] as $part => $details ) {
    if ( $details['type'] != 'markup' ) {
      $element['part-' . $part] = array(
        '#type' => 'textfield',
        '#size' => $details['length'],
        '#maxlength' => $details['length'],
        '#default_value' => $element['#default_value'][$part],
        '#attributes' => array(
          'class' => 'structured-text part-' . $part,
        ),
      );
      $js_ids[] = $part;
    }
    else {
      $element['part-' . $part] = array(
        '#type' => 'markup',
        '#value' => $details['value'],
        '#attributes' => array('class' => 'structured-text part-' . $part),
      );
    }
  }
  if ( count($js_ids) > 1 && $element['#auto_skip'] ) {
    foreach ( $js_ids as $index => $id ) {
      if ( isset($js_ids[$index + 1]) ) {
        $element['part-' . $id]['#attributes']['onkeyup'] = "webform_structured_text_jump('{$element['#id']}-part-$id',
          '{$element['#id']}-part-{$js_ids[$index + 1]}');";
      }
    }
    if ( !$js_added ) {
      $script = "function webform_structured_text_jump(from_id, to_id) {
                if ( event.keyCode != 9 && event.keyCode != 16 ) {
                  fromfield = $('#' + from_id);
                  if ( fromfield.val().length >= fromfield.attr('maxlength') ) {
                    $('#' + to_id).focus().select();
                  }
                }
              }";
      drupal_add_js($script, 'inline');
      $js_added = TRUE;
    }
  }
  return $element;
}

/**
 * Validation function for structured text.  Ensure that user input conforms to
 * the mask, and that no portions are left empty.
 *
 * @param array $element The the structured text element, including values.
 * @param array $form_state The state of the form.
 */
function webform_validate_structured_text($element, $form_state) {
  // Gather the actual input.
  $chunks = array();
  foreach ( $element['#mask'] as $part => $details ) {
    if ( $details['type'] != 'markup' ) {
      $chunks[$part] = $element['part-' . $part]['#value'];
    }
  }
  // Determine if any input fields are missing.  If they're all empty, then they're not missing.
  $numb_chunks = count($chunks);
  $numb_not_empty = count(array_filter($chunks));
  $numb_empty = $numb_chunks - $numb_not_empty;
  $missing_chunks = ( $numb_chunks != $numb_not_empty && $numb_chunks != $numb_empty );
  // Now evaluate each text field.
  $mask_regex_lines = explode("\n", $element['#mask_regex']);
  $mask_regex = array();
  foreach ( $mask_regex_lines as $line ) {
    list($chunk, $message, $regex) = explode('|', $line, 3);
    $mask_regex[$chunk] = array(
      'message' => $message,
      'regex' => $regex,
    );
  }
  $component = 0;  // Used to point the user to the correct chunk of the whole input field in error messages.
  foreach ( $chunks as $part => $value ) {
    $component++;
    $error_1 = $error_n = '';
    switch ( $element['#mask'][$part]['type'] ) {
      case '9':
        if ( ( $element['#required'] || $missing_chunks ) && $value == '' ||
                $value != '' && (
                        !is_numeric($value) || $value < 0  ||
                        strlen($value) < $element['#mask'][$part]['length'] ) ) {
          $error_1 = '!element field !component component must be 1 digit.';
          $error_n = '!element field !component component must be @count digits.';
        }
        break;

      case 'a':
        if ( ( $element['#required'] || $missing_chunks ) && $value == '' ||
                $value != '' && (
                        preg_match('/[^a-z]/i', $value)  ||
                        strlen($value) < $element['#mask'][$part]['length'] ) ) {
          $error_1 = '!element field !component component must be 1 alpha character (a-z).';
          $error_n = '!element field !component component must be @count alpha characters (a-z).';
        }
        break;

      case 'x':
        if ( ( $element['#required'] || $missing_chunks ) && $value == '' ||
                $value != '' && ( strlen($value) < $element['#mask'][$part]['length'] ) ) {
          $error_1 = '!element field !component component must be 1 character.';
          $error_n = '!element field !component component must be @count characters.';
        }
        break;
    }
    if ( $error_1 ) {
      $component_field = implode('][', array_merge($element['#parents'], array('part-' . $part,)));
      form_set_error($component_field,
              format_plural($element['#mask'][$part]['length'], $error_1, $error_n,
              array(
                '!element' => $element['#title'],
                '!component' => $component . _webform_structured_text_ordinal_suffix($component),
              )));
    }
    elseif ( ( $element['#required'] || !$missing_chunks && $numb_not_empty != 0 ) &&
            !empty($mask_regex[$component]['regex']) ) { // no errors so far, and we have a regex, so test it
      if ( !preg_match(trim($mask_regex[$component]['regex']), $value) ) {
        $component_field = implode('][', array_merge($element['#parents'], array('part-' . $part,)));
        form_set_error($component_field, check_plain($mask_regex[$component]['message']));
      }
    }
  }
}

/**
 * Theme a webform date element.
 */
function theme_webform_structured_text($element) {
  $output = '<div class="webform-container-inline">';
  foreach ( element_children($element) as $key ) {
    $output .= drupal_render($element[$key]);
  }
  $output .= '</div>';
  return theme('form_element', $element, $output);
}

/**
 * Implements _webform_display_component().
 */
function _webform_display_structured_text($component, $value, $format = 'html') {
  return array(
    '#title' => $component['name'],
    '#weight' => $component['weight'],
    '#theme' => 'webform_display_textfield',
    '#theme_wrappers' => $format == 'html' ? array('webform_element') : array('webform_element_text'),
    '#post_render' => array('webform_element_wrapper'),
    '#field_prefix' => $component['extra']['field_prefix'],
    '#field_suffix' => $component['extra']['field_suffix'],
    '#component' => $component,
    '#format' => $format,
    '#value' => $value[0] != '' ? webform_structured_text_format_value($component['extra']['mask'], $value[0], $format) : '',
  );
}


/**
 * Implements _webform_submit_component().
 */
function _webform_submit_structured_text($component, $value) {
  $combined_value = '';
  foreach ( $value as $input ) {
    $combined_value .= $input;
  }
  return $combined_value;
}

/**
 * Implements _webform_help_component().
 */
function _webform_help_structured_text($section) {
  switch ( $section ) {
    case 'admin/settings/webform#structured_text':
      return t('Allows creation of stuctured / input-masked text.');
  }
}


/**
 * Implements _webform_analysis_component.
 */
function _webform_analysis_structured_text($component, $sids = array(), $single = FALSE) {

  $query = db_select('webform_submitted_data', 'wsd', array('fetch' => PDO::FETCH_ASSOC))
    ->fields('wsd', array('sid', 'data',))
    ->condition('nid', $component['nid'])
    ->condition('cid', $component['cid']);

  if ( count($sids) ) {
    $query->condition('sid', $sids, 'IN');
  }
  $nonblanks = 0;
  $submissions = 0;
  $responses = array();

  $result = $query->execute();
  foreach ( $result as $data ) {
    if ( drupal_strlen(trim($data['data'])) > 0 ) {
      $nonblanks++;
      if ( $component['extra']['analysis_display'] ) {
        if ( $component['extra']['analysis_display_identifying'] == '' ) {
          $identifier = $data['sid'];
        }
        else {
          $identifier = db_query('SELECT data FROM {webform_submitted_data} WHERE nid = :nid AND cid = :cid AND sid = :sid LIMIT 1',
                  array(
                    ':nid' => $component['nid'],
                    ':cid' => $component['extra']['analysis_display_identifying'],
                    ':sid' => $data['sid'],
                  ))->fetchField();
        }
      $responses[] = array('<div class="webform-response">' . l($identifier, "node/{$component['nid']}/submission/{$data['sid']}") . ': ' .
        webform_structured_text_format_value($component['extra']['mask'], $data['data']) . '</div>', '');
      }
    }
    $submissions++;
  }

  $rows[0] = array(t('Left Blank'), ($submissions - $nonblanks));
  $rows[1] = array(t('User entered value'), $nonblanks);

  if ( $component['extra']['analysis_display'] ) {
    $rows = array_merge($rows, $responses);
  }

  return $rows;

}

/**
 * Implements _webform_table_component().
 */
function _webform_table_structured_text($component, $value) {
  return ( empty($value[0]) ? '' : webform_structured_text_format_value($component['extra']['mask'], $value[0]) );
}

/**
 * Implements _webform_cvs_headers_component.
 */
function _webform_csv_headers_structured_text($component, $export_options) {
  return array('', '', $component['name'],);
}

/**
 * Implements _webform_cvs_data_component().
 */
function _webform_csv_data_structured_text($component, $export_options, $value) {
  return ( empty($value[0]) ? '' : webform_structured_text_format_value($component['extra']['mask'], $value[0], 'text') );
}

/**
 * Helper function to parse a mask string into an array of mask parts.
 * @param string $mask The mask input by the user when the component was configured.
 * @return array Mask parts.
 */
function webform_structured_text_parse_mask($mask) {
  $mask_array = array();
  $mask = (string) $mask;
  $mask_length = drupal_strlen($mask);
  $where = '';
  $part = -1;
  for ( $i = 0 ; $i < $mask_length ; $i++ ) {
    $type = ( in_array($mask[$i], array('9', 'a', 'x',)) ? $mask[$i] : 'markup' );
    if ( $where != $type ) {
      $where = $type;
      $mask_array[++$part] = array(
        'type' => $type,
        'length' => 0,
        'value' => '',
      );
    }
    $mask_array[$part]['length']++;
    if ( $type == 'markup' ) {
      if ( $mask[$i] == '^' ) {
        if ( $i + 1 < $mask_length && in_array($mask[$i + 1], array('9', 'a', 'x', '^')) ) {
          $mask_array[$part]['value'] .= $mask[++$i];
        }
      }
      else
        $mask_array[$part]['value'] .= $mask[$i];
    }
  }
  return $mask_array;
}

/**
 * Helper function to take a string value and parse it into an array based on a mask.
 * @param mixed $mask Either the string mask, or the mask array.
 * @param string $value The string value to be parsed.
 * @return array The string $value broken into parts in an array, indexed by the part location.
 */
function webform_structured_text_parse_value($mask, $value) {
  $mask_array = is_array($mask) ? $mask : webform_structured_text_parse_mask($mask);
  $value_array = array();
  $start = 0;
  foreach ( $mask_array as $part => $details ) {
    if ( $details['type'] != 'markup' ) {
      $value_array[$part] = drupal_substr($value, $start, $details['length']);
      $start += $details['length'];
    }
  }
  return $value_array;
}

/**
 * Helper function to format a value per a mask.
 * @param mixed $mask Either the string mask, or the mask array.
 * @param string $value The value to be formatted.
 * @param string $format What format (HTML or text) should be returned.
 *        Specifying 'html' forces a pass of the output through check_plain.
 * @return string The formatted value.
 */
function webform_structured_text_format_value($mask, $value, $format = 'html') {
  $mask_array = is_array($mask) ? $mask : webform_structured_text_parse_mask($mask);
  $output = '';
  $start = 0;
  foreach ( $mask_array as $details ) {
    if ( $details['type'] != 'markup' ) {
      $output .= substr($value, $start, $details['length']);
      $start += $details['length'];
    }
    else {
      $output .= $details['value'];
    }
  }
  return ( $format == 'html' ? check_plain($output) : $output );
}

/**
 * Helper function to generate the description for the control, optionally indluding
 * input format instructions / description.
 * @param type $component
 * @return string The component's description
 */
function webform_structured_text_field_description($component) {
  $description = $component['extra']['description'];
  if ( $component['extra']['mask_description'] ) {
    $mask_array = webform_structured_text_parse_mask($component['extra']['mask']);
    $output = array();
    foreach ( $mask_array as $details ) {
      switch ( $details['type'] ) {
        case '9':
          $output[] = format_plural($details['length'], '1 digit', '@count digits');
          break;
        case 'a':
          $output[] = format_plural($details['length'], '1 alpha (a-z) character', '@count alpha (a-z) characters');
          break;
        case 'x':
          $output[] = format_plural($details['length'], '1 character', '@count characters');
          break;
        default:
          break;
      }
    }
    if ( !empty($output) ) {
      $last = array_pop($output);
      $output = ( !empty($output) ? implode(', ', $output) . ', ' . t('and') . ' ' . $last : $last );
      $description .= ' <strong>' . t('Input format:') . '</strong> ' . $output . '.';
    }
  }
  return $description;
}

/**
 * Helper function to display the ordinal ending for an integer.
 * @param int $num The number for which the ordinal ending is desired.
 * @return string The ordinal ending.
 */
function _webform_structured_text_ordinal_suffix($num){
    if($num < 11 || $num > 13){
         switch($num % 10){
            case 1: return 'st';
            case 2: return 'nd';
            case 3: return 'rd';
        }
    }
    return 'th';
}


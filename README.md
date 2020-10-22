Webform Structured Text
=======================

This module adds a new type of Webform component: structured / masked-input text.
Specify an input mask, and the form input will be rendered to accept that mask.

This module provides a webform component for structured text - text that follows
an input-mask format. For example, you may want to gather phone numbers in a
particular format, but an ordinary text field doesn't do that. You could cobble
together a few different text or number fields to do what you want, and use
Webform Validation to try to check the input, but that's a pain to maintain.
This control provides you with a single field definition and data storage,
with validation based on the mask and optionally supplied RegEx.

Installation
------------

- Install this module using the official Backdrop CMS instructions at
  <https://backdropcms.org/guide/modules>

How to use
----------

- Go to the main field list of a webform.
- Add a structured text element to your form.
- In the component settings, specify an input mask.
- Save the component.
- The component will be rendered as distinct text fields and markup per your mask.

The component validates that all chunks of the field are filled in according
to the specified mask if the field is required or if any chuck has input.
Validation errors are generated if chunks are missing, or the characters in
the chunk don't correspond to the mask, or if any specified RegEx comparisons
fail.

Once the input passes validation, the chunks are concatenated and stored as a
single value in the database. However, they are displayed and output to CSV in
the format specified by the mask.

Theming
-------

The default behaviour when rendering a structured text component is to make
the component appear as if it is a single text field with the individual input
boxes inside this field. This is done by using a surrounding `<div>` with a
border around it, and then making the individual `<input>` fields borderless.
Because of this, the rendered component may not look exactly like the rest of
your text input fields, particularly if they are heavily styled. The
webform_structured_text.css file has instructions for themers who want to modify
the look of the component to match the rest of their `<input>` fields. Note that
this only applies if you allow Webform Structured Text to render the component
to appear as a single field. If you instead choose to render the component as
individual boxes for each chunk, they will render as do all your other text
input fields.

Issues
------

Bugs and Feature requests should be reported in the Issue Queue:
https://github.com/backdrop-contrib/webform_structured_text/issues

Current Maintainers
-------------------

- Herb v/d Dool <https://github.com/herbdool>

Credits
-------

- Ported to Backdrop by Herb v/d Dool <https://github.com/herbdool>
- Originally developed for Drupal by sdsheridan <https://www.drupal.org/u/sdsheridan>.

License
-------

This project is GPL v2 software. See the LICENSE.txt file in this directory for
complete text.

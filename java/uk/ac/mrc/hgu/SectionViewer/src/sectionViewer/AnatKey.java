package sectionViewer;
import sectionViewer.*;

import java.util.*;
import java.awt.*;
import java.awt.event.*;
import javax.swing.*;
import javax.swing.event.*;
import java.io.*;
import java.lang.reflect.Method;
import java.lang.reflect.InvocationTargetException;

import uk.ac.mrc.hgu.Wlz.*;

/**
 *   A table which indicates the colour of anatomy components in SectionViewers.
 *   AnatKey has controls to allow the colour to be changed
 *   and the component visibility to be toggled off / on.
 *   AnatKey is a singleton class.
 */
public class AnatKey extends AnatKeyGUI{

   /**
    *   The one and only instance of AnatKey (per parent).
    */
   protected AnatKey _instance = null;

   /**
    *   The collection of rows in the AnatKey.
    */
   protected  Vector _keyEntryVec = null;

   /**
    *   The number of rows in the AnatKey.
    */
   protected  int _nrows = 0;

   /**
    *   A unique index number for KeyElements.
    */
   protected  int _indx = 0;

//-------------------------------------------------------------
   /**
    *   Constructs a 2D AnatKey with the default title.
    *   Only 1 instance is allowed per parent.
    *   It is the responsibility of the parent to ensure
    *   that only 1 instance is constructed. !!!
    */
   public AnatKey() {
      this(false);
   }

//............................
   /**
    *   Constructs an AnatKey with the default title
    *   and 2D / 3D as specified.
    *   It is the responsibility of the parent to ensure
    *   that only 1 instance is constructed. !!!
    */
   public AnatKey(boolean is3D) {

      super("Anatomy Key", is3D);
      _is3D = is3D;
      _keyEntryVec = new Vector();
   }

//-------------------------------------------------------------
   /**
    *
    */
   public void addRow(String txt) {

      Dimension currDim = null;
      currDim = this.getSize(null);
      this.setSize(new Dimension(currDim.width,
                                 currDim.height + KeyEntry.getH() + 2));
      KeyEntry row = new KeyEntry(_indx, _is3D);
      _keyEntryVec.add(row);
      _nrows++;
      row.setText(txt);
      row.setCol(nextCol());
      kTopPanel.add(row);
      this.invalidate();
      this.validate();
   }
//-------------------------------------------------------------
   /**
    *
    */
   public void removeRow(int indx) {

      Dimension currDim = null;
      currDim = this.getSize(null);
      this.setSize(new Dimension(currDim.width,
                                 currDim.height - KeyEntry.getH() - 2));
      /* find row with appropriate indx from _keyEntryVec */
      KeyEntry row = getRow(indx);
      _keyEntryVec.remove(row);
      kTopPanel.remove(row);
      this.invalidate();
      this.validate();
      _nrows--;
      if(_nrows == 0) {
	 this.setSize(new Dimension(currDim.width, _emptyH));
      }
   }
//-------------------------------------------------------------
   /**
    *   Returns the number of rows in the anatomy key.
    *   @return The int number of rows in the key.
    */
   public int getNRows() {
      return _nrows;
   }

//-------------------------------------------------------------
   /**
    *   Sets the visibility of a KeyEntry to the given state.
    *   @param indx the index of the KeyEntry to be made visible.
    *   @param str the full name of the corresponding anatomy component.
    *   @param viz true if the KeyEntry is to be visible.
    */
   public void setEntryVisible(int indx, boolean viz) {

      KeyEntry row = this.getRow(indx);
      row.setEntryVisible(viz);

   }

//-------------------------------------------------------------
   /**
    *   Sets the initial colour of the anatomy component
    *   to 1 of 6 distinct colours.
    *   @return the next colour.
    */
   public Color nextCol() {

      int i = (_ncols + _indx -1) % _ncols;
      return new Color(_rgbt[i][0],
                       _rgbt[i][1],
                       _rgbt[i][2]);
   }

//-------------------------------------------------------------
   /**
    *   Sets the colour of the anatomy component
    *   to that returned by a standard color chooser dialog.
    *   The color chooser dialog is opened by clicking on the 
    *   Colour chooser button in the AnatKey.
    *   @param col the new colour of the anatomy component.
    *   @param indx the position (i.e. the row number in the AnatKey)
    *   of the anatomy component whose colour is being changed.
    */
   public void setCol(int indx) {

      Color col = null;
      KeyEntry row = null;

      row = getRow(indx);

      if(row != null) {
	 col = JColorChooser.showDialog(null,
		     "choose colour for anatomy component",
		     row.getCol());

	 if(col != null) {
	    row.setCol(col);
	 }
      }

   }

//-------------------------------------------------------------
   /**
    *   Returns the colour of an anatomy component at
    *   the given index in the Anatomy Key.
    *   @param indx the index.
    *   @return a new Color object.
    */
   public Color getColor(int indx){

      Color col = null;
      KeyEntry row = null;

      row = getRow(indx);
      if(row != null) {
	col = row.getCol();
      }

      return col;
   }

//-------------------------------------------------------------
   /**
    *   Resets the AnatKey to its empty state.
    */
   public void reset() {

      int num = _keyEntryVec.size();
      int indxs[] = new int[num];
      KeyEntry row = null;

      /* first get the collection of index numbers */
      for(int i = 0; i<num; i++) {
         row = (KeyEntry)_keyEntryVec.elementAt(i);
	 indxs[i] = row.getIndx();
      }
      row = null;

      /* then remove each row */
      for(int j = 0; j<num; j++) {
         removeRow(indxs[j]);
      }
   }

//-------------------------------------------------------------
   /**
    *   Returns the keyEntry with the specified indx.
    *   @param indx a unique index for each keyEntry.
    *   @return the KeyEntry with the specified indx.
    */
   public KeyEntry getRow(int indx) {

      int i = -1;
      int size = _keyEntryVec.size();
      for(i=0; i<size; i++) {
	 if(((KeyEntry)_keyEntryVec.elementAt(i)).getIndx() == indx) {
	    break;
	 }
      }
      return (KeyEntry)_keyEntryVec.elementAt(i);
   }

//-------------------------------------------------------------
   /**
    *   Increments the unique index and returns the new value.
    *   @return _indx.
    */
   public int nextIndx() {
      _indx++;
      return _indx;
   }

//-------------------------------------------------------------
   /**
    *   Sets all the 3D visibility control icons.
    *   @param state true if controls are to have viz3DIcon.
    */
   public void set3DVisIcons(boolean visible) {
     int size = _keyEntryVec.size();
     for(int i=0; i<size; i++) {
        ((KeyEntry)_keyEntryVec.elementAt(i)).set3DVisIcon(visible);
     }
   }

//-------------------------------------------------------------
// handle all objects that are interested in changes
//-------------------------------------------------------------

   // keep track of all the listeners to this model
   /**
    *   A list of ActionListeners which are
    *   listening for events fired from the AnatKey.
    */
/*
   protected EventListenerList actionListeners =
                             new EventListenerList();
*/

  // add a listener to the register
   /**
    *   Registers an ActionListener
    *   with the EventListenerList.
    *   @param x an Event handler implementing ActionListener
    */
/*
  public void addActionListener(ActionListener x) {
    actionListeners.add (ActionListener.class, x);
  }
*/


  // remove a listener from the register
   /**
    *   Removes a previously registered ActionListener
    *   from the EventListenerList
    *   @param x an Event handler implementing ActionListener
    */
/*
  public void removeActionListener(ActionListener x) {
    actionListeners.remove (ActionListener.class, x);
  }
*/

   /**
    *   Fires an ActionEvent with the given Action Command.
    *   This allows an event handler to choose the appropriate action.
    */
/*
   protected void fireAction(String cmd) {
   // Create the event:
   ActionEvent ae = new ActionEvent(this,
                                    ActionEvent.ACTION_PERFORMED,
				    cmd);
   // Get the listener list
   Object[] listeners =
     actionListeners.getListenerList();
   // Process the listeners last to first
   // List is in pairs, Class and instance
   for (int i
     = listeners.length-2; i >= 0; i -= 2) {
     if (listeners[i] == ActionListener.class) {
        ActionListener al = (ActionListener)listeners[i+1];
        al.actionPerformed(ae);
     }
   }
  } // fireAction
*/

//-------------------------------------------------------------

   /**
    *   For testing only.
    */
/*
  public void main(String argv[]) {

	AnatKey _key;

        _key = AnatKey.instance();
	_key.setSize(300,500);
	_key.pack();
	_key.setVisible(true);

  }
*/

} // class AnatKey

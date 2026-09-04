Trifecta SDK
================================

Development SDK for the Trifecta series of IMU/AHRS/INS devices.

The Trifecta SDK is a portable C driver for the **Intelligent Sensor Fusion**
device family (Trifecta-K, Trifecta-M, and related models). It provides device
discovery, initialization, data streaming, packet decoding, device
configuration, data logging, and offline replay of logged data.

Usage and redistribution of this code is permitted but this notice must be
retained in all copies of the code.

.. toctree::
   :maxdepth: 2
   :caption: Contents:

   getting_started
   discovery
   initialization
   streaming
   packets
   configuration
   saving
   replay
   types
   porting
   api_reference

Additional resources
--------------------

- **Source repository:** `Trifecta-Driver <https://github.com/Triangle-Man-LLC/Trifecta-Driver>`_
- **License:** see the ``LICENSE`` file in the repository root.

.. todo::

   Examples for the supported platforms (ESP-IDF, Linux, Windows, Android)
   will be added here once the examples directory is finalized.

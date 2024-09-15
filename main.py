import streamlit as st

# Initialize session state for access and page navigation
if 'rfid_access' not in st.session_state:
    st.session_state.rfid_access = None  # None means the access hasn't been checked yet
if 'auth2_matched' not in st.session_state:
    st.session_state.auth2_matched = None  # None means authentication 2 hasn't been checked yet

# Create a manual input for RFID access (True or False)
rfid_access = st.radio("Select RFID Access Status", [True, False], index=0)

# Simulate the logic for Authentication 2 (True or False)
auth2_matched = st.radio("Select Authentication 2 Status", [True, False], index=0)

# Update session state based on manual input
if rfid_access:
    st.session_state.rfid_access = True
else:
    st.session_state.rfid_access = False

if auth2_matched:
    st.session_state.auth2_matched = True
else:
    st.session_state.auth2_matched = False

# Page 1: If RFID access is not yet determined, show the scan page with NFC image
if st.session_state.rfid_access is None:
    st.header("Please Scan Your ID")
    st.image("nfc.jpg", caption="Place your card on the NFC scanner", use_column_width=True)  # Display NFC symbol

# Page 2: Based on the RFID access result
elif st.session_state.rfid_access:
    st.header("Look at the camera")  # Access granted, show camera prompt
    st.info("Authentication 1 has completed, Authentication 2 is pending")  # Additional message for access granted

# Page 3: Based on Authentication 2 result
if st.session_state.rfid_access is not None and st.session_state.auth2_matched is not None:
    if st.session_state.rfid_access and st.session_state.auth2_matched:
        st.image("Lock_open (2).png", caption="Lock is opened", use_column_width=True)  # Show open image
    else:
        st.image("lock_closed (2).png", caption="Access Denied", use_column_width=True)  # Show closed image

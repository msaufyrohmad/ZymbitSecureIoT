/**
 * @file zk_app_utils.h
 * @author Scott Miller
 * @version 1.0
 * @date November 17, 2016
 * @copyright Zymbit, Inc.
 * @brief C interface to Zymkey Application Utilities Library.
 * @details
 * This file contains the C API to the the Zymkey Application Utilities
 * library. This API facilitates writing user space applications which use
 * Zymkey to perform cryptographic operations, such as:
 *      1. Signing of payloads using ECDSA
 *      2. Verification of payloads that were signed using Zymkey
 *      3. Exporting the public key that matches Zymkey's private key
 *      4. "Locking" and "unlocking" data objects
 *      5. Generating random data
 * Additionally, there are functions for changing the i2c address (i2c units
 * only), setting tap sensitivity and controlling the LED.
 */

#ifndef __ZK_APP_UTILS_H
#define __ZK_APP_UTILS_H

#ifdef __cplusplus
extern "C"
{
#endif // __cplusplus

#include <stdbool.h>
#include <stdint.h>

/**
 * @typedef The typedef for the Zymkey Context type.
 */
typedef void* zkCTX;

/**
 * @brief Supported key types for signature validation against foreign public
 *        keys
 */
typedef enum ZK_FOREIGN_PUBKEY_TYPE
{
    ZK_FOREIGN_PUBKEY_NISTP256,
    ZK_FOREIGN_PUBKEY_SECP256K1,
} ZK_FOREIGN_PUBKEY_TYPE;

/**
 * @brief Accelerometer axis enum, used to set tap sensitivity.
 */
typedef enum ZK_ACCEL_AXIS_TYPE
{
    ZK_ACCEL_AXIS_X,
    ZK_ACCEL_AXIS_Y,
    ZK_ACCEL_AXIS_Z,
    ZK_ACCEL_AXIS_ALL
} ZK_ACCEL_AXIS_TYPE;

/**
 * @brief zkGetAccelerometer data output.
 *
 */
typedef struct zkAccelAxisDataType
{
    double g;           /**< the axis reading in units of g-force */
    int tapDirection;   /**< the direction of the force along the axis which caused a tap event:
                          * -1 = negative
                          * +1 = positive
                          * 0  = did not cause a tap event
                          */
} zkAccelAxisDataType;

/**
 * @brief Perimeter breach action flag definitions.
 */
#define ZK_PERIMETER_EVENT_ACTION_NOTIFY        (1 << 0)
#define ZK_PERIMETER_EVENT_ACTION_SELF_DESTRUCT (1 << 1)

/*
 *  Zymkey context open/close.
 */

/**
 * @brief Open a Zymkey context.
 * @param ctx
 *        (output) returns a pointer to a Zymkey context.
 * @return 0 for success, less than 0 for failure.
 */
int zkOpen(zkCTX* ctx);

/**
 * @brief Close a Zymkey context.
 * @param ctx
 *        (input) The Zymkey context to close
 * @return 0 for success, less than 0 for failure.
 */
int zkClose(zkCTX ctx);

/*
 *  Random number generation.
 */

/**
 * @brief Fill a file with random numbers.
 * @param ctx
 *        (input) Zymkey context.
 * @param dst_filename
 *        (input) Absolute path name for the destination file.
 * @param rdata_sz
 *        (input) The number of random bytes to generate.
 * @return 0 for success, less than 0 for failure.
 */
int zkCreateRandDataFile(zkCTX ctx, const char* dst_filename, int rdata_sz);

/**
 * @brief Get an array of random bytes.
 * @param ctx
 *        (input) Zymkey context.
 * @param rdata
 *        (input) Pointer to a pointer of bytes.
 * @param rdata_sz
 *        (input) The number of random bytes to generate.
 * @return 0 for success, less than 0 for failure.
 */
int zkGetRandBytes(zkCTX ctx, uint8_t** rdata, int rdata_sz);

/*
 *  Lock data
 */

/**
 * @brief Lock up source (plaintext) data from a file and store the results
 * (ciphertext) in a destination file
 * @details
 *   This function encrypts and signs a block of plaintext data from a file
 *   and stores the result in a destination file.
 * @note
 *   The zymkey has two keys that can be used for locking/unlocking
 *   operations, designated as 'shared' and 'one-way'.
 *     1. The one-way key is meant to lock up data only on the
 *        local host computer. Data encrypted using this key cannot
 *        be exported and deciphered anywhere else.
 *     2. The shared key is meant for publishing data to other
 *        sources that have the capability to generate the shared
 *        key, such as the Zymbit cloud server.
 *
 * @param ctx
 *        (input) Zymkey context.
 * @param src_pt_filename
 *        (input) The absolute path to the file where the source (plaintext) data
 *        is located.
 * @param dst_ct_filename
 *        (input) The absolute path to the file where the destination (ciphertext)
 *        data should be deposited.
 * @param use_shared_key
 *        (input) This parameter specifies which key will be used to
 *        used to lock the data up. A value of 'false' specifies that the Zymkey
 *        will use the one-way key whereas 'true' specifies that the shared key
 *        will be used. Specify 'true' for publishing data to another  that has the
 *        shared key (e.g. Zymbit cloud) and 'False' when the data is meant to
 *        reside exclusively withing the host computer.
 * @return 0 for success, less than 0 for failure.
 */
int zkLockDataF2F(zkCTX ctx,
                  const char* src_pt_filename,
                  const char* dst_ct_filename,
                  bool use_shared_key);

/**
 * @brief Lock up source (plaintext) data from a byte array and store the results
 * (ciphertext) in a destination file
 * @details
 *   This function encrypts and signs a block of binary plaintext data
 *   and stores the result in a destination file.
 * @note (See zkLockDataF2F for notes about keys)
 *
 * @param ctx
 *        (input) Zymkey context.
 * @param src_pt
 *        (input) Binary plaintext source byte array.
 * @param src_pt_sz
 *        (input) Size of plaintext source data.
 * @param dst_ct_filename
 *        (input) The absolute path to the file where the destination (ciphertext)
 *        data should be deposited.
 * @param use_shared_key
 *        (input) Specifies if shared key is to be used. See zkLockDataF2F.
 * @return 0 for success, less than 0 for failure.
 */
int zkLockDataB2F(zkCTX ctx,
                  const uint8_t* src_pt,
                  int src_pt_sz,
                  const char* dst_ct_filename,
                  bool use_shared_key);

/**
 * @brief Lock up source (plaintext) data from a file and store the results
 * (ciphertext) in a destination byte array.
 * @details
 *   This function encrypts and signs a block of plaintext data from a file
 *   and stores the result in a binary byte array.
 * @note (See zkLockDataF2F for notes about keys)
 *
 * @param ctx
 *        (input) Zymkey context.
 * @param src_pt_filename
 *        (input) The absolute path to the file where the source (plaintext) data
 *        is located.
 * @param dst_ct
 *        (output) A pointer to a pointer to an array of unsigned bytes created by
 *        this function. This pointer must be freed by the application when no longer
 *        needed.
 * @param dst_ct_sz
 *        (output) A pointer to an integer which contains the size of the
 *        destination array.
 * @param use_shared_key
 *        (input) Specifies if shared key is to be used. See zkLockDataF2F.
 * @return 0 for success, less than 0 for failure.
 */
int zkLockDataF2B(zkCTX ctx,
                  const char* src_pt_filename,
                  uint8_t** dst_ct,
                  int* dst_ct_sz,
                  bool use_shared_key);

/**
 * @brief Lock up source (plaintext) data from a byte array and store the results
 * (ciphertext) in a destination byte array.
 * @details
 *   This function encrypts and signs a block of plaintext data and stores the
 *   result in a binary byte array.
 * @note (See zkLockDataF2F for notes about keys)
 *
 * @param ctx
 *        (input) Zymkey context.
 * @param src_pt
 *        (input) Binary plaintext source byte array.
 * @param src_pt_sz
 *        (input) Size of plaintext source data.
 * @param dst_ct
 *        (output) A pointer to a pointer to an array of unsigned bytes created by
 *        this function. This pointer must be freed by the application when no longer
 *        needed.
 * @param dst_ct_sz
 *        (output) A pointer to an integer which contains the size of the
 *        destination array.
 * @param use_shared_key
 *        (input) Specifies if shared key is to be used. See zkLockDataF2F.
 * @return 0 for success, less than 0 for failure.
 */
int zkLockDataB2B(zkCTX ctx,
                  const uint8_t* src_pt,
                  int src_pt_sz,
                  uint8_t** dst_ct,
                  int* dst_ct_sz,
                  bool use_shared_key);

/*
 *  Unlock data
 */

/**
 * @brief Unlock source (ciphertext) data from a file and store the results
 * (plaintext) in a destination file
 * @details
 *   This function verifies a locked object signature and decrypts the
 *   associated ciphertext data.
 * @note (See zkLockDataF2F for notes about keys)
 *
 * @param ctx
 *        (input) Zymkey context.
 * @param src_ct_filename
 *        (input) The absolute path to the file where the source (ciphertext) data
 *        is located.
 * @param dst_pt_filename
 *        (input) The absolute path to the file where the destination (plaintext)
 *        data should be deposited.
 * @param use_shared_key
 *        (input) This parameter specifies which key will be used to
 *        used to lock the data up. A value of 'false' specifies that the Zymkey
 *        will use the one-way key whereas 'true' specifies that the shared key
 *        will be used. Specify 'true' for publishing data to another  that has the
 *        shared key (e.g. Zymbit cloud) and 'False' when the data is meant to
 *        reside exclusively withing the host computer.
 * @return 0 for success, less than 0 for failure.
 */
int zkUnlockDataF2F(zkCTX ctx,
                    const char* src_ct_filename,
                    const char* dst_pt_filename,
                    bool use_shared_key);

/**
 * @brief Unlock source (ciphertext) data from a byte array and store the results
 * (plaintext) in a destination file
 * @details
 *   This function verifies a locked object signature and decrypts the
 *   associated ciphertext data.
 * @note (See zkLockDataF2F for notes about keys)
 *
 * @param ctx
 *        (input) Zymkey context.
 * @param src_ct
 *        (input) Binary ciphertext source byte array.
 * @param src_ct_sz
 *        (input) Size of ciphertext source data.
 * @param dst_pt_filename
 *        (input) The absolute path to the file where the destination (plaintext)
 *        data should be deposited.
 * @param use_shared_key
 *        (input) Specifies if shared key is to be used. See zkLockDataF2F.
 * @return 0 for success, less than 0 for failure.
 */
int zkUnlockDataB2F(zkCTX ctx,
                    const uint8_t* src_ct,
                    int src_ct_sz,
                    const char* dst_pt_filename,
                    bool use_shared_key);

/**
 * @brief Unlock source (ciphertext) data from a file and store the results
 * (plaintext) in a destination byte array.
 * @details
 *   This function verifies a locked object signature and decrypts the
 *   associated ciphertext data.
 * @note (See zkLockDataF2F for notes about keys)
 *
 * @param ctx
 *        (input) Zymkey context.
 * @param src_ct_filename
 *        (input) The absolute path to the file where the source (ciphertext) data
 *        is located.
 * @param dst_pt
 *        (output) A pointer to a pointer to an array of unsigned bytes created by
 *        this function. This pointer must be freed by the application when no longer
 *        needed.
 * @param dst_pt_sz
 *        (output) A pointer to an integer which contains the size of the
 *        destination array.
 * @param use_shared_key
 *        (input) Specifies if shared key is to be used. See zkLockDataF2F.
 * @return 0 for success, less than 0 for failure.
 */
int zkUnlockDataF2B(zkCTX ctx,
                    const char* src_ct_filename,
                    uint8_t** dst_pt,
                    int* dst_pt_sz,
                    bool use_shared_key);

/**
 * @brief Unlock source (ciphertext) data from a byte array and store the results
 * (plaintext) in a destination byte array.
 * @details
 *   This function verifies a locked object signature and decrypts the
 *   associated ciphertext data.
 * @note (See zkLockDataF2F for notes about keys)
 *
 * @param ctx
 *        (input) Zymkey context.
 * @param src_ct
 *        (input) Binary ciphertext source byte array.
 * @param src_ct_sz
 *        (input) Size of ciphertext source data.
 * @param dst_pt
 *        (output) A pointer to a pointer to an array of unsigned bytes created by
 *        this function. This pointer must be freed by the application when no longer
 *        needed.
 * @param dst_pt_sz
 *        (output) A pointer to an integer which contains the size of the
 *        destination array.
 * @param use_shared_key
 *        (input) Specifies if shared key is to be used. See zkLockDataF2F.
 * @return 0 for success, less than 0 for failure.
 */
int zkUnlockDataB2B(zkCTX ctx,
                    const uint8_t* src_ct,
                    int src_ct_sz,
                    uint8_t** dst_pt,
                    int* dst_pt_sz,
                    bool use_shared_key);

/*
 *  ECDSA
 */

/**
 * @brief Generate a signature using the Zymkey's ECDSA private key.
 * @param ctx
 *        (input) Zymkey context.
 * @param digest
 *        (input) This parameter contains the digest of the data that
 *        will be used to generate the signature.
 * @param slot
 *        (input) The key slot to generate a signature from. This parameter is
 *        only valid for Zymkey models 4i and beyond.
 * @param sig
 *        (output) A pointer to a pointer to an array of unsigned bytes which
 *        contains the generated signature. This pointer is created by this function
 *        and must be freed by the application when no longer needed.
 * @param sig_sz
 *        (output) A pointer to an integer which contains the size of the signature.
 * @return 0 for success, less than 0 for failure.
 */
int zkGenECDSASigFromDigest(zkCTX ctx,
                            const uint8_t* digest,
                            int slot,
                            uint8_t** sig,
                            int* sig_sz);

/**
 * @brief Verify a signature using the Zymkey's ECDSA public key.
 *    The public is not specified in the parameter list to insure
 *    that the public key that matches the Zymkey's ECDSA private key
 *    is used.
 * @param ctx
 *        (input) Zymkey context.
 * @param digest
 *        (input) This parameter contains the digest of the data that
 *        will be used to generate the signature.
 * @param slot
 *        (input) The key slot to generate a signature from. This parameter is
 *        only valid for Zymkey models 4i and beyond.
 * @param sig
 *        (input) Array of bytes which contains the signature.
 * @param sig_sz
 *        (input) Size of signature.
 * @return 0 for signature verification failed, 1 for signature verification
 *         passed, less than 0 for general failure.
 */
int zkVerifyECDSASigFromDigest(zkCTX ctx,
                               const uint8_t* digest,
                               int slot,
                               const uint8_t* sig,
                               int sig_sz);

/**
 * @brief Verify a signature using a foreign ECDSA public key.
 * @param ctx
 *        (input) Zymkey context.
 * @param digest
 *        (input) This parameter contains the digest of the data that
 *        will be used to generate the signature.
 * @param foreign_pubkey
 *        (input) A byte array in binary representation of the foreign public
 *        key which is paired to the private key which was used to generate
 *        the signature. This array must contain the uncompressed specifier
 *        which means that the preamble byte of '0x04' must be in the first
 *        byte. For example, a public key corresponding to a 256 bit EC curve
 *        would be 65 bytes long.
 * @param foreign_pubkey_size
 *        (input) The foreign public key size
 * @param sig
 *        (input) Array of bytes which contains the signature.
 * @param sig_sz
 *        (input) Size of signature.
 * @param sig_is_der
 *        (input) If the input signature is in DER format, set to true.
 * @param ec_curve_type
 *        (input) Type of curve to verify against. Maps to ZK_FOREIGN_PUBKEY_TYPE.
 * @return 0 for signature verification failed, 1 for signature verification
 *         passed, less than 0 for general failure.
 */
int zkVerifyECDSASigFromDigestWithForeignKey(zkCTX ctx,
                                             const uint8_t* digest,
                                             const uint8_t* foreign_pubkey,
                                             int foreign_pubkey_sz,
                                             const uint8_t* sig,
                                             int sig_sz,
                                             bool sig_is_der,
                                             int ec_curve_type);

/**
 * @brief Store the ECDSA public key to a file in PEM format.
 * @details This function is useful for generating Certificate Signing Requests
 *          (CSR).
 * @param ctx
 *        (input) Zymkey context.
 * @param filename
 *        (input) Filename where PEM formatted public key is to be stored.
 * @param slot
 *        (input) The key slot to retrieve. Only valid for model 4i and above.
 * @return 0 for success, less than 0 for failure.
 */
int zkSaveECDSAPubKey2File(zkCTX ctx,
                           const char* filename,
                           int slot);

/**
 * @brief Gets the ECDSA public key and stores in a byte array created by this
 *        function.
 * @param ctx
 *        (input) Zymkey context.
 * @param pk
 *        (output) Pointer to a pointer created by this function which contains
 *        the public key.
 * @param pk_sz
 *        (output) Pointer to an integer which contains the size of the public
 *        key.
 * @param slot
 *        (input) The key slot to retrieve. Only valid for model 4i and above.
 * @return 0 for success, less than 0 for failure.
 */
int zkGetECDSAPubKey(zkCTX ctx,
                     uint8_t** pk,
                     int* pk_sz,
                     int slot);
/*
 * LED control
 */

/**
 * @brief Turns the LED off.
 * @param ctx
 *        (input) Zymkey context.
 * @return 0 for success, less than 0 for failure.
 */
int zkLEDOff(zkCTX ctx);

/**
 * @brief Turns the LED on.
 * @param ctx
 *        (input) Zymkey context.
 * @return 0 for success, less than 0 for failure.
 */
int zkLEDOn(zkCTX ctx);

/**
 * @brief Flashes the LED.
 * @param ctx
 *        (input) Zymkey context.
 * @param on_ms
 *        (input) The amount of time, in milliseconds, that the LED will stay
 *        on during a flash cycle.
 * @param off_ms
 *        (input) The amount of time, in milliseconds, that the LED will stay
 *        off during a flash cycle.
 * @param num_flashes
 *        (input) The number of on/off flash cycles to complete. If this
 *        parameter is 0, then the LED will flash indefinitely.
 * @return 0 for success, less than 0 for failure.
 */
int zkLEDFlash(zkCTX ctx,
               uint32_t on_ms,
               uint32_t off_ms,
               uint32_t num_flashes);

/*
 * Administrative Ops
*/

/**
 * @brief Sets the i2c address of the Zymkey (i2c versions only)
 * @details This method should be called if the i2c address of the
 *    Zymkey is shared with another i2c device on the same i2c bus.
 *    The default i2c address for Zymkey units is 0x30. Currently,
 *    the address may be set in the ranges of 0x30 - 0x37 and
 *    0x60 - 0x67.
 *    After successful completion of this command, the Zymkey will
 *    reset itself.
 * @param addr
 *        (input) The i2c address that the Zymkey will set itself to.
 * @return 0 for success, less than 0 for failure.
 */
int zkSetI2CAddr(zkCTX ctx, int addr);

/*
 * Time
 */

/**
 * @brief Get current GMT time
 * @details This function is called to get the time directly from a
 *          Zymkey's Real Time Clock (RTC)
 * @param epoch_time_sec
 *        (output) The time in seconds from the epoch (Jan. 1, 1970).
 * @param precise_time
 *        (input) If true, this API returns the time after the next second
 *        falls. This means that the caller could be blocked up to one second.
 *        If false, the API returns immediately with the current time reading.
 * @return 0 for success, less than 0 for failure.
 */
int zkGetTime(zkCTX ctx, uint32_t* epoch_time_sec, bool precise_time);

/*
 * Accelerometer
 */

/**
 * @brief Sets the sensitivity of tap operations.
 *  @details This method permits setting the sensitivity of the tap
 *           detection feature. Each axis may be individually
 *           configured or all at once.
 * @param axis
 *        (input) The axis to configure. This parameter should contain
 *        one of the values in the enum typedef ACCEL_AXIS_TYPE.
 * @param pct
 *        (input) The sensitivity expressed as percentage.
 *        1. 0% = Shut down: Tap detection should not occur along the
 *           axis.
 *        2. 100% = Maximum sensitivity.
 * @return 0 for success, less than 0 for failure.
 */
int zkSetTapSensitivity(zkCTX ctx, int axis, float pct);

/**
 * @brief Wait for a tap event to be detected
 * @details This function is called in order to wait for a tap event to occur.
 *          This function blocks the calling thread unless called with a
 *          timeout of zero.
 * @param timeout_ms
 *        (input) The maximum amount of time in milliseconds to wait for a tap
 *        event to arrive.
 * @return 0 for success, less than 0 for failure, -ETIMEDOUT when no tap
 *         events detected within the specified timeout
 */
int zkWaitForTap(zkCTX ctx, uint32_t timeout_ms);

/**
 * @brief Get current accelerometer data and tap info.
 * @details This function gets the most recent accelerometer data in units of g
 *          forces plus the tap direction per axis.
 * @param x
 *        (output) x axis accelerometer information
 *        y
 *        (output) y axis accelerometer information
 *        z
 *        (output) z axis accelerometer information
 * @return 0 for success, less than 0 for failure.
 */
int zkGetAccelerometerData(zkCTX ctx, zkAccelAxisDataType* x, zkAccelAxisDataType* y, zkAccelAxisDataType* z);

/*
 * Perimeter detect
 */

/**
 * @brief Wait for a perimeter breach event to be detected
 * @details This function is called in order to wait for a perimeter breach
 *          event to occur. This function blocks the calling thread unless
 *          called with a timeout of zero. Note that, in order to receive
 *          perimeter events, the zymkey must have been configured to notify
 *          the host on either or both of the perimeter detect channels via a
 *          call to "zkSetPerimeterEventAction".
 * @param timeout_ms
 *        (input) The maximum amount of time in milliseconds to wait for a
 *        perimeter event to arrive.
 * @return 0 for success, less than 0 for failure, -ETIMEDOUT when no perimeter
 *         events detected within the specified timeout
 */
int zkWaitForPerimeterEvent(zkCTX ctx, uint32_t timeout_ms);

/**
 * @brief Get current perimeter detect info.
 * @details This function gets the timestamp of the first perimeter detect
 *          event for the given channel
 * @param timestamps_sec
 *        (output) The timestamps for when the event occurred. 0 if no events
 *                 have occurred.
 *         num_timestamps
 *         (output) The number of timestamps in the returned array
 * @return 0 for success, less than 0 for failure.
 */
int zkGetPerimeterDetectInfo(zkCTX ctx, uint32_t** timestamps_sec, int* num_timestamps);

/**
 * @brief Clear perimeter detect events.
 * @details This function clears all perimeter detect event info and rearms all
 *          perimeter detect channels
 * @return 0 for success, less than 0 for failure.
 */
int zkClearPerimeterDetectEvents(zkCTX ctx);

/**
 * @brief Set perimeter breach action
 * @details This function specifies the action to take when a perimeter breach
 *          event occurs. The possible actions are any combination of:
 *              1. Notify host
 *              2. Zymkey self-destruct
 * @param channel
 *        (input) The channel that the action flags will be applied to
 *        action_flags
 *        (input) The actions to apply to the perimeter event channel:
 *                1. Notify (ZK_PERIMETER_EVENT_ACTION_NOTIFY)
 *                2. Self-destruct (ZK_PERIMETER_EVENT_ACTION_SELF_DESTRUCT)
 * @return 0 for success, less than 0 for failure.
 */
int zkSetPerimeterEventAction(zkCTX ctx, int channel, uint32_t action_flags);

#ifdef __cplusplus
}
#endif // __cplusplus

#endif // __ZK_APP_UTILS_H

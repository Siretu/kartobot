package se.exuvo.kartobotpc.comm;

import com.rapplogic.xbee.api.XBeeAddress16;
import com.rapplogic.xbee.api.XBeeAddress64;
import com.rapplogic.xbee.api.zigbee.ZBNodeDiscover;
import com.rapplogic.xbee.api.zigbee.ZBNodeDiscover.DeviceType;
import com.rapplogic.xbee.api.zigbee.ZNetNodeIdentificationResponse;
import com.rapplogic.xbee.util.ByteUtils;

public class ZNetNode {

	private XBeeAddress64 nodeAddress64;
	private XBeeAddress16 nodeAddress16;
	private String nodeIdentifier;
	private XBeeAddress16 parentAddress;
	private DeviceType deviceType;
	private int[] profileId;
	private int[] mfgId;
	
	public ZNetNode(XBeeAddress64 a64, XBeeAddress16 a16){
		setNodeAddress64(a64);
		setNodeAddress16(a16);
	}
	
	public ZNetNode(ZBNodeDiscover nd){
		setNodeAddress64(nd.getNodeAddress64());
		setNodeAddress16(nd.getNodeAddress16());
		setNodeIdentifier(nd.getNodeIdentifier());
		setParentAddress(nd.getParent());
		setDeviceType(nd.getDeviceType());
		setProfileId(nd.getProfileId());
		setMfgId(nd.getMfgId());
	}
	
	public ZNetNode(ZNetNodeIdentificationResponse ni){
		setNodeAddress64(ni.getRemoteAddress64());
		setNodeAddress16(ni.getRemoteAddress16());
		setNodeIdentifier(ni.getNodeIdentifier());
		setParentAddress(ni.getParentAddress());
		setDeviceType(DeviceType.get(ni.getDeviceType().getValue()));
		setProfileId(new int[]{ni.getProfileId().getMsb(), ni.getProfileId().getLsb()});
		setMfgId(new int[]{ni.getMfgId().getMsb(), ni.getMfgId().getLsb()});
	}
	
	public XBeeAddress64 getNodeAddress64() {
		return nodeAddress64;
	}

	public void setNodeAddress64(XBeeAddress64 nodeAddress64) {
		this.nodeAddress64 = nodeAddress64;
	}

	public XBeeAddress16 getNodeAddress16() {
		return nodeAddress16;
	}

	public void setNodeAddress16(XBeeAddress16 nodeAddress16) {
		this.nodeAddress16 = nodeAddress16;
	}

	public String getNodeIdentifier() {
		return nodeIdentifier;
	}

	public void setNodeIdentifier(String nodeIdentifier) {
		this.nodeIdentifier = nodeIdentifier;
	}

	public XBeeAddress16 getParentAddress() {
		return parentAddress;
	}

	public void setParentAddress(XBeeAddress16 parentAddress) {
		this.parentAddress = parentAddress;
	}

	public DeviceType getDeviceType() {
		return deviceType;
	}

	public void setDeviceType(DeviceType deviceType) {
		this.deviceType = deviceType;
	}

	public int[] getProfileId() {
		return profileId;
	}

	public void setProfileId(int[] profileId) {
		this.profileId = profileId;
	}

	public int[] getMfgId() {
		return mfgId;
	}

	public void setMfgId(int[] mfgId) {
		this.mfgId = mfgId;
	}
	
	public String toString() {
		return "nodeAddress16=" + this.nodeAddress16 + 
		", nodeAddress64=" + this.nodeAddress64 + 
		", nodeIdentifier=" + this.nodeIdentifier + 
		", parentAddress=" + this.getParentAddress() + 
		", deviceType=" + this.deviceType +
		", profileId=" + ByteUtils.toBase16(this.profileId) + 
		", mfgId=" + ByteUtils.toBase16(this.mfgId);
	}
	
}
